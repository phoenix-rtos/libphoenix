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
#include <phoenix/ethtool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "ioctl-helper.h"

#define IOCTL_PACK(type, ioctl_struct, out_ptr, subptr_name, sub_struct_size, total_size) \
	do { \
		*total_size = sizeof(type) + sub_struct_size; \
		if (*total_size > IOCPARM_MASK) { \
			return -EINVAL; \
		} \
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

#define ETHTOOL_STRUCT_SIZE_LEN_DATA_NAMES_OPS(type, ptr, len_field_name, data_field_name, len_field_op, data_field_op) \
	(sizeof(type) + ((len_field_op(((type *)(ptr))->len_field_name)) * (data_field_op(((type *)(ptr))->data_field_name[0]))))

#define ETHTOOL_STRUCT_SIZE_LEN_DATA_NAMES_LEN_OP(type, ptr, len_field_name, data_field_name, len_field_op) \
	ETHTOOL_STRUCT_SIZE_LEN_DATA_NAMES_OPS(type, ptr, len_field_name, data_field_name, len_field_op, sizeof)

#define ETHTOOL_STRUCT_SIZE_LEN_DATA_NAMES(type, ptr, len_field_name, data_field_name) \
	ETHTOOL_STRUCT_SIZE_LEN_DATA_NAMES_OPS(type, ptr, len_field_name, data_field_name, /* empty */, sizeof)

#define ETHTOOL_STRUCT_SIZE_LEN_NAME(type, ptr, len_field_name) \
	ETHTOOL_STRUCT_SIZE_LEN_DATA_NAMES(type, ptr, len_field_name, data)

#define ETHTOOL_STRUCT_SIZE_DATA_NAME(type, ptr, data_field_name) \
	ETHTOOL_STRUCT_SIZE_LEN_DATA_NAMES(type, ptr, len, data_field_name)

#define ETHTOOL_STRUCT_SIZE_DEFAULT(type, ptr) ETHTOOL_STRUCT_SIZE_LEN_NAME(type, ptr, len)


static inline ssize_t ioctl_packEthtool(const struct ifreq *ifr, void **out_ptr, size_t *out_size)
{
	size_t ethtool_struct_size;
	void *ethtool_struct = ifr->ifr_data;

	if (ethtool_struct == NULL) {
		return EFAULT;
	}

	/* first field of every ethtool struct is always uint32_t cmd */
	uint32_t cmd = *((uint32_t *)ethtool_struct);

	switch (cmd) {
		case ETHTOOL_GSET:
		case ETHTOOL_SSET:
			ethtool_struct_size = sizeof(struct ethtool_cmd);
			break;

		case ETHTOOL_TEST:
			ethtool_struct_size = ETHTOOL_STRUCT_SIZE_DEFAULT(struct ethtool_test, ethtool_struct);
			break;

		case ETHTOOL_GSTRINGS:
			ethtool_struct_size = sizeof(struct ethtool_gstrings) + (((struct ethtool_gstrings *)ethtool_struct)->len * ETH_GSTRING_LEN);
			break;

		case ETHTOOL_GSSET_INFO:
			/* length of data is [the number of bits set in sset_mask] * [sizeof(*data)] */
			ethtool_struct_size = ETHTOOL_STRUCT_SIZE_LEN_DATA_NAMES_LEN_OP(struct ethtool_sset_info, ethtool_struct, sset_mask, data, __builtin_popcountll);
			break;

		case ETHTOOL_GLINKSETTINGS:
		case ETHTOOL_SLINKSETTINGS:
			ethtool_struct_size = ETHTOOL_STRUCT_SIZE_LEN_DATA_NAMES(struct ethtool_link_settings, ethtool_struct, link_mode_masks_nwords, link_mode_masks);
			break;

		default:
			return -EOPNOTSUPP;
	}

	IOCTL_PACK(struct ifreq, ifr, out_ptr, ifr_data, ethtool_struct_size, out_size);

	return 0;
}


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
		case SIOCETHTOOL:
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

		case SIOCETHTOOL:
			ret = ioctl_packEthtool(ioctl_val, out_ptr, out_size);
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

		case SIOCETHTOOL:
			IOCTL_UNPACK(struct ifreq, ioctl_val, in_ptr, ifr_data, size);
			break;

		default:
			return;
	}

	ioctl_free(request, in_ptr);
}
