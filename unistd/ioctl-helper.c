/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * ioctl special case input serialization/deserialization
 *
 * Copyright 2025 Phoenix Systems
 * Author: Julian Uziemblo
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include "ioctl-helper.h"

#define PTR_ADD(ptr, offset)           ((void *)(((uint8_t *)(ptr)) + (offset)))
#define PTR_ADD_AND_DEREF(ptr, offset) (*(void **)PTR_ADD(ptr, offset))
#define SIZEOF_ARRAY(arr)              (sizeof(arr) / sizeof(arr[0]))


typedef struct {
	const size_t offset;
	/* returns: < 0 on error; 0 when no serialization needed; > 0 the size of subptr */
	ssize_t (*const getSize)(const void *val);
} subptr_t;


typedef struct {
	const subptr_t *const subptrs;
	const size_t nsubptrs;
} nestedLayout_t;


typedef struct {
	const unsigned long request;
	void **packed;
	size_t *packedSize;
	void *ioctlVal;
	size_t ioctlValSize;
	const nestedLayout_t *layout;
} ioctl_ctx_t;


typedef int (*ioctl_op_t)(ioctl_ctx_t *ctx);


/*
 * Extendable support for nested structures in ioctl commands
 *
 * To support new nested ioctl:
 * 1) Define `<ioctl-name>Subptrs[]` - an array of `subptr_t` structures
 *    representing the inner layout of the nested ioctl structure. Each
 *    entry corresponds to one sub-pointer and describes its offset in
 *    the structure and provides a function to calculate the size of the
 *    pointed-to structure (it shall return: 0 when no serialization is
 *    needed, negative value on error and positive size value on success).
 * 2) Define `<ioctl-name>Layout`. This structure consists of a pointer
 *    to the defined above sub-pointers array and its size.
 * 3) Add a branch to the switch statement in `ioctl_dispatchOp` function
 *    with the ioctl's request number as a matcher that assigns created
 *    layout structure to the `layout` pointer variable.
 */


static ssize_t ioctl_rtDevSize(const void *val)
{
	const struct rtentry *rt = val;
	if (rt->rt_dev == NULL) {
		return -EINVAL;
	}
	return strlen(rt->rt_dev) + 1;
}


static const subptr_t rtentrySubptrs[] = { {
	.offset = offsetof(struct rtentry, rt_dev),
	.getSize = ioctl_rtDevSize,
} };


static const nestedLayout_t rtentryLayout = {
	.subptrs = rtentrySubptrs,
	.nsubptrs = SIZEOF_ARRAY(rtentrySubptrs),
};


static ssize_t ioctl_ifcBufLen(const void *val)
{
	const struct ifconf *ifc = val;
	size_t ifc_buf_len = ifc->ifc_len;
	if (ifc->ifc_buf == NULL) {
		if (ifc_buf_len == 0) {
			/* device returns the needed length
			   -> no packing/unpacking needed */
			return 0;
		}
		else {
			return -EINVAL;
		}
	}

	return ifc_buf_len;
}


static const subptr_t ifconfSubptrs[] = { {
	.offset = offsetof(struct ifconf, ifc_buf),
	.getSize = ioctl_ifcBufLen,
} };


static const nestedLayout_t ifconfLayout = {
	.subptrs = ifconfSubptrs,
	.nsubptrs = SIZEOF_ARRAY(ifconfSubptrs),
};


static inline int ioctl_dispatchOp(unsigned long request, void **packed, size_t *packedSize, void *ioctlVal, size_t ioctlValSize, ioctl_op_t op)
{
	/* IOC_NESTED checked before this in ioctl() */
	const nestedLayout_t *layout;

	switch (request) {
		case SIOCADDRT:
		case SIOCDELRT:
			layout = &rtentryLayout;
			break;

		case SIOCGIFCONF:
			layout = &ifconfLayout;
			break;

		default:
			return -EOPNOTSUPP;
	}

	ioctl_ctx_t ctx = {
		.request = request,
		.packed = packed,
		.packedSize = packedSize,
		.ioctlVal = ioctlVal,
		.ioctlValSize = ioctlValSize,
		.layout = layout,
	};

	return op(&ctx);
}


/*
 * Common routines
 */


static inline ssize_t ioctl_totalSizeNeeded(ioctl_ctx_t *ctx)
{
	ssize_t size;
	size_t totalSize = ctx->ioctlValSize;

	for (size_t i = 0; i < ctx->layout->nsubptrs; i++) {
		size = ctx->layout->subptrs[i].getSize(ctx->ioctlVal);
		if (size < 0) {
			return size;
		}
		totalSize += size;
	}

	return totalSize;
}


static int ioctl_opSerialize(ioctl_ctx_t *ctx)
{
	size_t totalSize;
	size_t offset = 0;

	ssize_t size = ioctl_totalSizeNeeded(ctx);
	if (size < 0) {
		return size;
	}
	totalSize = size;

	if (totalSize == ctx->ioctlValSize) {
		/* no packing needed - ioctl is already the right size */
		return 0;
	}

	void *mem = malloc(totalSize);
	if (mem == NULL) {
		return -ENOMEM;
	}

	(void)memcpy(PTR_ADD(mem, offset), ctx->ioctlVal, ctx->ioctlValSize);
	offset += ctx->ioctlValSize;

	for (size_t i = 0; i < ctx->layout->nsubptrs; i++) {
		size = ctx->layout->subptrs[i].getSize(ctx->ioctlVal);
		if (size < 0) {
			free(mem);
			return size;
		}
		(void)memcpy(PTR_ADD(mem, offset), PTR_ADD_AND_DEREF(ctx->ioctlVal, ctx->layout->subptrs[i].offset), size);
		offset += size;
	}

	*ctx->packed = mem;
	*ctx->packedSize = totalSize;

	return 0;
}


static int ioctl_opDeserialize(ioctl_ctx_t *ctx)
{
	if (*ctx->packed == NULL) {
		/* no unpacking needed - val wasn't packed */
		return 0;
	}

	size_t offset = ctx->ioctlValSize;
	ssize_t size;

	for (size_t i = 0; i < ctx->layout->nsubptrs; i++) {
		size = ctx->layout->subptrs[i].getSize(ctx->ioctlVal);
		if (size < 0) {
			return size;
		}
		if (size == 0) {
			continue;
		}
		/* copy old pointer value to helper variable */
		void *subptr = PTR_ADD_AND_DEREF(ctx->ioctlVal, ctx->layout->subptrs[i].offset);

		if (subptr != NULL) {
			/* set pointer value in allocated structure to copy the right pointer at the end */
			PTR_ADD_AND_DEREF(*ctx->packed, ctx->layout->subptrs[i].offset) = subptr;

			/* copy current substructure */
			(void)memcpy(subptr, PTR_ADD(*ctx->packed, offset), size);
			offset += size;
		}
	}

	/* copy back the main struct */
	(void)memcpy(ctx->ioctlVal, *ctx->packed, ctx->ioctlValSize);

	return 0;
}


static int ioctl_opFree(ioctl_ctx_t *ctx)
{
	if (*ctx->packed != NULL) {
		free(*ctx->packed);
	}
	return 0;
}


static int ioctl_opGetPointerField(ioctl_ctx_t *ctx)
{
	ssize_t size;
	size_t offset = IOCPARM_LEN(ctx->request);

	for (size_t i = 0; i < ctx->layout->nsubptrs; i++) {
		if (ctx->layout->subptrs[i].offset == ctx->ioctlValSize) {
			*ctx->packed = PTR_ADD(ctx->ioctlVal, offset);
			return 0;
		}
		size = ctx->layout->subptrs[i].getSize(ctx->ioctlVal);
		if (size < 0) {
			return size;
		}
		offset += size;
	}

	return -ENODATA;
}


void ioctl_free(unsigned long request, void *packed)
{
	(void)ioctl_dispatchOp(request, &packed, NULL, NULL, 0, ioctl_opFree);
}


int ioctl_serialize(unsigned long request, void *ioctlVal, void **packed, size_t *packedSize)
{
	/* set to NULL so if we don't allocate the memory, we won't call free on it */
	*packed = NULL;
	return ioctl_dispatchOp(request, packed, packedSize, ioctlVal, IOCPARM_LEN(request), ioctl_opSerialize);
}


void ioctl_deserialize(unsigned long request, void *ioctlVal, void *packed)
{
	if (ioctl_dispatchOp(request, &packed, NULL, ioctlVal, IOCPARM_LEN(request), ioctl_opDeserialize) != -EOPNOTSUPP) {
		ioctl_free(request, packed);
	}
}


int ioctl_getPointerField(unsigned long request, void **subptr, const void *val, size_t offset)
{
	/* ioctl_opGetPointerField doesn't change val: const signalizes to user that val will not be changed */
	return ioctl_dispatchOp(request, subptr, NULL, (void *)val, offset, ioctl_opGetPointerField);
}
