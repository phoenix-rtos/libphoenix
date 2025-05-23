/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * ioctl special case input serialization/deserialization
 *
 * Copyright 2025 Phoenix Systems
 * Author: Julian Uziembło
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "ioctl-helper.h"

#define IOCTL_PACK(type, ioctl_struct, out_ptr, subptr_name, sub_struct_size, total_size) \
	do { \
		*total_size = sizeof(type) + sub_struct_size; \
		void *_tmp = malloc(*total_size); \
		if (_tmp == NULL) { \
			return -ENOMEM; \
		} \
		*out_ptr = _tmp; \
		memcpy(_tmp, ioctl_struct, sizeof(type)); \
		_tmp = (char *)_tmp + sizeof(type); \
		memcpy(_tmp, ioctl_struct->subptr_name, sub_struct_size); \
	} while (0)

#define IOCTL_UNPACK(type, ioctl_struct, in_ptr, subptr_name, total_size) \
	do { \
		type *_tmp = ioctl_struct; \
		void *_tmp_subptr = _tmp->subptr_name; \
		memcpy(_tmp, in_ptr, sizeof(type)); \
		memcpy(_tmp_subptr, (char *)in_ptr + sizeof(type), total_size - sizeof(type)); \
		_tmp->subptr_name = _tmp_subptr; \
	} while (0)


static inline int ioctl_packRtEntry(const struct rtentry *rt, void **out_ptr, size_t *out_size)
{
	size_t rt_dev_size = 0;
	if (rt->rt_dev != NULL) {
		rt_dev_size = strlen(rt->rt_dev);
	}

	IOCTL_PACK(struct rtentry, rt, out_ptr, rt_dev, rt_dev_size, out_size);

	return 0;
}


static inline int ioctl_packIfconf(const struct ifconf *ifc, void **out_ptr, size_t *out_size)
{
	size_t ifc_buf_len = 0;
	if (ifc->ifc_buf != NULL) {
		ifc_buf_len = ifc->ifc_len;
	}

	IOCTL_PACK(struct ifconf, ifc, out_ptr, ifc_buf, ifc_buf_len, out_size);

	return 0;
}


static void ioctl_free(unsigned long request, void *alloced)
{
	/* sanity check */
	switch (request) {
		case SIOCGIFCONF:
		case SIOCADDRT:
		case SIOCDELRT:
			if (alloced != NULL) {
				free(alloced);
			}
			break;

		default:
			break;
	}
}


int ioctl_serialize(unsigned long request, void *ioctl_val, void **out_ptr, size_t *out_size)
{
	int ret;

	/* set to NULL so if we don't allocate the memory, we won't call free on it */
	*out_ptr = NULL;

	switch (request) {
		case SIOCADDRT:
		case SIOCDELRT:
			ret = ioctl_packRtEntry(ioctl_val, out_ptr, out_size);
			break;

		case SIOCGIFCONF:
			ret = ioctl_packIfconf(ioctl_val, out_ptr, out_size);
			break;

		default:
			*out_ptr = ioctl_val;
			*out_size = IOCPARM_LEN(request);
			ret = 0;
			break;
	}

	if (ret < 0) {
		ioctl_free(request, *out_ptr);
	}

	return ret;
}


void ioctl_deserialize(unsigned long request, void *ioctl_val, void *in_ptr, size_t size)
{
	switch (request) {
		case SIOCADDRT:
		case SIOCDELRT:
			IOCTL_UNPACK(struct rtentry, ioctl_val, in_ptr, rt_dev, size);
			break;

		case SIOCGIFCONF:
			IOCTL_UNPACK(struct ifconf, ioctl_val, in_ptr, ifc_buf, size);
			break;

		default:
			return;
	}

	ioctl_free(request, in_ptr);
}
