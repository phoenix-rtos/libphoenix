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
#include <phoenix/ethtool.h>

#include "ioctl-helper.h"

#define PTR_ADD(ptr, offset)           ((void *)(((uint8_t *)(ptr)) + (offset)))
#define PTR_ADD_AND_DEREF(ptr, offset) (*(void **)PTR_ADD(ptr, offset))
#define SIZEOF_ARRAY(arr)              (sizeof(arr) / sizeof(arr[0]))


typedef struct {
	size_t offset;
	int (*getSize)(void *val, size_t *sizeOut);
} subptr_info_t;


typedef struct {
	unsigned long request;
	void **packed;
	size_t *packedSize;
	void *ioctlVal;
	size_t ioctlValSize;
	const subptr_info_t *subptrs;
	size_t nsubptrs;
} ioctl_ctx_t;


typedef int (*ioctl_op_t)(ioctl_ctx_t *ctx);


static int ioctl_ethtool(void *val, size_t *sizeOut)
{
	struct ifreq *ifr = val;
	size_t ethtoolSize;
	void *ethtool = ifr->ifr_data;

	if (ethtool == NULL) {
		return -EINVAL;
	}

	/* first field of every ethtool struct is always uint32_t cmd */
	uint32_t cmd = *((uint32_t *)ethtool);

	switch (cmd) {
		case ETHTOOL_GSET:
		case ETHTOOL_SSET:
			ethtoolSize = sizeof(struct ethtool_cmd);
			break;

		case ETHTOOL_TEST: {
			struct ethtool_test *test = ethtool;
			ethtoolSize = sizeof(*test) + (test->len * sizeof(test->data[0]));
			break;
		}

		case ETHTOOL_GSTRINGS: {
			struct ethtool_gstrings *gstrings = ethtool;
			ethtoolSize = sizeof(*gstrings) + (gstrings->len * ETH_GSTRING_LEN);
			break;
		}

		case ETHTOOL_GSSET_INFO: {
			struct ethtool_sset_info *sset = ethtool;
			ethtoolSize = sizeof(*sset) + (__builtin_popcountll(sset->sset_mask) * sizeof(sset->data[0]));
			break;
		}

		case ETHTOOL_GLOOPBACK:
		case ETHTOOL_SLOOPBACK:
			ethtoolSize = sizeof(struct ethtool_value);
			break;

		case ETHTOOL_GLINKSETTINGS:
		case ETHTOOL_SLINKSETTINGS: {
			struct ethtool_link_settings *settings = ethtool;
			ethtoolSize = sizeof(*settings) + (settings->link_mode_masks_nwords * sizeof(settings->link_mode_masks[0]));
			break;
		}

		default:
			return -EOPNOTSUPP;
	}

	*sizeOut = ethtoolSize;
	return 0;
}


static const subptr_info_t ethtoolSubptrs[] = { {
	.offset = __builtin_offsetof(struct ifreq, ifr_data),
	.getSize = ioctl_ethtool,
} };


static int ioctl_rtentry(void *val, size_t *sizeOut)
{
	struct rtentry *rt = val;
	size_t rt_dev_size = 0;
	if (rt->rt_dev == NULL) {
		return -EINVAL;
	}
	rt_dev_size = strlen(rt->rt_dev) + 1;

	*sizeOut = rt_dev_size;
	return 0;
}


static const subptr_info_t rtentrySubptrs[] = { {
	.offset = __builtin_offsetof(struct rtentry, rt_dev),
	.getSize = ioctl_rtentry,
} };


static int ioctl_ifconf(void *val, size_t *sizeOut)
{
	struct ifconf *ifc = val;
	size_t ifc_buf_len = ifc->ifc_len;
	if (ifc->ifc_buf == NULL) {
		if (ifc_buf_len == 0) {
			/* device returns the needed length
			   -> no packing/unpacking needed */
			*sizeOut = 0;
			return 0;
		}
		else {
			return -EINVAL;
		}
	}

	*sizeOut = ifc_buf_len;
	return 0;
}


static const subptr_info_t ifconfSubptrs[] = { {
	.offset = __builtin_offsetof(struct ifconf, ifc_buf),
	.getSize = ioctl_ifconf,
} };


static const struct reqinfo {
	unsigned long request;
	const subptr_info_t *subptrs;
	size_t nsubptrs;
} requestInfo[] = {
	{
		.request = SIOCGIFCONF,
		.subptrs = ifconfSubptrs,
		.nsubptrs = SIZEOF_ARRAY(ifconfSubptrs),
	},
	{
		.request = SIOCADDRT,
		.subptrs = rtentrySubptrs,
		.nsubptrs = SIZEOF_ARRAY(rtentrySubptrs),
	},
	{
		.request = SIOCDELRT,
		.subptrs = rtentrySubptrs,
		.nsubptrs = SIZEOF_ARRAY(rtentrySubptrs),
	},
	{
		.request = SIOCETHTOOL,
		.subptrs = ethtoolSubptrs,
		.nsubptrs = SIZEOF_ARRAY(ethtoolSubptrs),
	},
};


static inline int ioctl_totalSizeNeeded(ioctl_ctx_t *ctx, size_t *sizeOut)
{
	int err;
	size_t size;
	size_t totalSize = ctx->ioctlValSize;

	for (size_t i = 0; i < ctx->nsubptrs; i++) {
		err = ctx->subptrs[i].getSize(ctx->ioctlVal, &size);
		if (err < 0) {
			return err;
		}
		totalSize += size;
	}

	*sizeOut = totalSize;
	return 0;
}


static int ioctl_opSerialize(ioctl_ctx_t *ctx)
{
	if (ctx->nsubptrs == 0 || ctx->subptrs == NULL) {
		/* nothing to pack */
		*ctx->packed = ctx->ioctlVal;
		*ctx->packedSize = ctx->ioctlValSize;
		return 0;
	}

	size_t totalSize;
	size_t offset = 0;
	size_t size;
	int err = ioctl_totalSizeNeeded(ctx, &totalSize);
	if (err < 0) {
		return err;
	}

	void *mem = malloc(totalSize);
	if (mem == NULL) {
		return -ENOMEM;
	}

	(void)memcpy(PTR_ADD(mem, offset), ctx->ioctlVal, ctx->ioctlValSize);
	offset += ctx->ioctlValSize;

	for (size_t i = 0; i < ctx->nsubptrs; i++) {
		err = ctx->subptrs[i].getSize(ctx->ioctlVal, &size);
		if (err < 0) {
			free(mem);
			return err;
		}
		(void)memcpy(PTR_ADD(mem, offset), PTR_ADD_AND_DEREF(ctx->ioctlVal, ctx->subptrs[i].offset), size);
		offset += size;
	}

	*ctx->packed = mem;
	*ctx->packedSize = totalSize;

	return 0;
}


static int ioctl_opDeserialize(ioctl_ctx_t *ctx)
{
	void *from = *ctx->packed;
	size_t offset = ctx->ioctlValSize;
	size_t size;
	int err;

	if (ctx->nsubptrs == 0 || ctx->subptrs == NULL) {
		/* nothing to unpack */
		(void)memcpy(ctx->ioctlVal, from, ctx->ioctlValSize);
		return 0;
	}

	for (size_t i = 0; i < ctx->nsubptrs; i++) {
		err = ctx->subptrs[i].getSize(ctx->ioctlVal, &size);
		if (err < 0) {
			return err;
		}
		/* copy old pointer value to helper variable */
		void *subptr = PTR_ADD_AND_DEREF(ctx->ioctlVal, ctx->subptrs[i].offset);
		/* set pointer value in allocated structure to copy the right pointer at the end */
		PTR_ADD_AND_DEREF(from, ctx->subptrs[i].offset) = subptr;

		/* copy current substructure */
		(void)memcpy(subptr, PTR_ADD(from, offset), size);
		offset += size;
	}

	/* copy back the main struct */
	(void)memcpy(ctx->ioctlVal, from, ctx->ioctlValSize);

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
	int err;
	size_t size;
	size_t offset = IOCPARM_LEN(ctx->request);

	for (size_t i = 0; i < ctx->nsubptrs; i++) {
		if (ctx->subptrs[i].offset == ctx->ioctlValSize) {
			*ctx->packed = PTR_ADD(ctx->ioctlVal, offset);
			return 0;
		}
		err = ctx->subptrs[i].getSize(ctx->ioctlVal, &size);
		if (err < 0) {
			return err;
		}
		offset += size;
	}

	return -ENODATA;
}


static inline int ioctl_dispatchOp(unsigned long request, void **packed, size_t *packedSize, void *ioctlVal, size_t ioctlValSize, ioctl_op_t op)
{
	for (size_t i = 0; i < sizeof(requestInfo) / sizeof(requestInfo[0]); i++) {
		const struct reqinfo *info = &requestInfo[i];
		if (request == info->request) {
			ioctl_ctx_t ctx = {
				.request = request,
				.packed = packed,
				.packedSize = packedSize,
				.ioctlVal = ioctlVal,
				.ioctlValSize = ioctlValSize,
				.subptrs = info->subptrs,
				.nsubptrs = info->nsubptrs,
			};
			return op(&ctx);
		}
	}
	return -EOPNOTSUPP;
}


void ioctl_free(unsigned long request, void *packed)
{
	(void)ioctl_dispatchOp(request, &packed, NULL, NULL, 0, ioctl_opFree);
}


int ioctl_serialize(unsigned long request, void *ioctlVal, void **packed, size_t *packedSize)
{
	int ret = 0;

	/* set to NULL so if we don't allocate the memory, we won't call free on it */
	*packed = NULL;

	ret = ioctl_dispatchOp(request, packed, packedSize, ioctlVal, IOCPARM_LEN(request), ioctl_opSerialize);
	if (ret == -EOPNOTSUPP) {
		*packed = ioctlVal;
		*packedSize = IOCPARM_LEN(request);
		ret = 0;
	}

	return ret;
}


void ioctl_deserialize(unsigned long request, void *ioctlVal, void *packed)
{
	if (ioctl_dispatchOp(request, &packed, NULL, ioctlVal, IOCPARM_LEN(request), ioctl_opDeserialize) == -EOPNOTSUPP) {
		return;
	}
	ioctl_free(request, packed);
}


int ioctl_getPointerField(unsigned long request, void **subptr, void *val, size_t offset)
{
	return ioctl_dispatchOp(request, subptr, NULL, val, offset, ioctl_opGetPointerField);
}
