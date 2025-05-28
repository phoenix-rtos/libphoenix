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

		case ETHTOOL_GDRVINFO:
			ethtool_struct_size = sizeof(struct ethtool_drvinfo);
			break;

		case ETHTOOL_GREGS:
			ethtool_struct_size = ETHTOOL_STRUCT_SIZE_DEFAULT(struct ethtool_regs, ethtool_struct);
			break;

		case ETHTOOL_GWOL:
		case ETHTOOL_SWOL:
			ethtool_struct_size = sizeof(struct ethtool_wolinfo);
			break;

		case ETHTOOL_GEEPROM:
		case ETHTOOL_SEEPROM:
		case ETHTOOL_GMODULEEEPROM:
			ethtool_struct_size = ETHTOOL_STRUCT_SIZE_DEFAULT(struct ethtool_eeprom, ethtool_struct);
			break;

		case ETHTOOL_GCOALESCE:
		case ETHTOOL_SCOALESCE:
			ethtool_struct_size = sizeof(struct ethtool_coalesce);
			break;

		case ETHTOOL_GRINGPARAM:
		case ETHTOOL_SRINGPARAM:
			ethtool_struct_size = sizeof(struct ethtool_ringparam);
			break;

		case ETHTOOL_GPAUSEPARAM:
		case ETHTOOL_SPAUSEPARAM:
			ethtool_struct_size = sizeof(struct ethtool_pauseparam);
			break;

		case ETHTOOL_TEST:
			ethtool_struct_size = ETHTOOL_STRUCT_SIZE_DEFAULT(struct ethtool_test, ethtool_struct);
			break;

		case ETHTOOL_GSTRINGS:
			ethtool_struct_size = sizeof(struct ethtool_gstrings) + (((struct ethtool_gstrings *)ethtool_struct)->len * ETH_GSTRING_LEN);
			break;

		case ETHTOOL_GSTATS:
		case ETHTOOL_GPHYSTATS:
			ethtool_struct_size = ETHTOOL_STRUCT_SIZE_LEN_NAME(struct ethtool_stats, ethtool_struct, n_stats);
			break;

		case ETHTOOL_GPERMADDR:
			ethtool_struct_size = ETHTOOL_STRUCT_SIZE_LEN_NAME(struct ethtool_perm_addr, ethtool_struct, size);
			break;

		case ETHTOOL_GRXFH:
		case ETHTOOL_SRXFH:
		case ETHTOOL_GRXRINGS:
		case ETHTOOL_GRXCLSRLCNT:
		case ETHTOOL_GRXCLSRULE:
		case ETHTOOL_GRXCLSRLALL:
		case ETHTOOL_SRXCLSRLDEL:
		case ETHTOOL_SRXCLSRLINS:
			ethtool_struct_size = ETHTOOL_STRUCT_SIZE_LEN_DATA_NAMES(struct ethtool_rxnfc, ethtool_struct, rule_cnt, rule_locs);
			break;

		case ETHTOOL_FLASHDEV:
			ethtool_struct_size = sizeof(struct ethtool_flash);
			break;

		case ETHTOOL_SRXNTUPLE:
			ethtool_struct_size = sizeof(struct ethtool_rx_ntuple);
			break;

		case ETHTOOL_GSSET_INFO:
			/* length of data is [the number of bits set in sset_mask] * [sizeof(*data)] */
			ethtool_struct_size = ETHTOOL_STRUCT_SIZE_LEN_DATA_NAMES_LEN_OP(struct ethtool_sset_info, ethtool_struct, sset_mask, data, __builtin_popcountll);
			break;

		case ETHTOOL_GRXFHINDIR:
		case ETHTOOL_SRXFHINDIR:
			ethtool_struct_size = ETHTOOL_STRUCT_SIZE_LEN_DATA_NAMES(struct ethtool_rxfh_indir, ethtool_struct, size, ring_index);
			break;

		case ETHTOOL_GFEATURES:
			ethtool_struct_size = ETHTOOL_STRUCT_SIZE_LEN_DATA_NAMES(struct ethtool_gfeatures, ethtool_struct, size, features);
			break;

		case ETHTOOL_SFEATURES:
			ethtool_struct_size = ETHTOOL_STRUCT_SIZE_LEN_DATA_NAMES(struct ethtool_sfeatures, ethtool_struct, size, features);
			break;

		case ETHTOOL_GCHANNELS:
		case ETHTOOL_SCHANNELS:
			ethtool_struct_size = sizeof(struct ethtool_channels);
			break;

		case ETHTOOL_SET_DUMP:
		case ETHTOOL_GET_DUMP_FLAG:
		case ETHTOOL_GET_DUMP_DATA:
			ethtool_struct_size = ETHTOOL_STRUCT_SIZE_DEFAULT(struct ethtool_dump, ethtool_struct);
			break;

		case ETHTOOL_GET_TS_INFO:
			ethtool_struct_size = sizeof(struct ethtool_ts_info);
			break;

		case ETHTOOL_GMODULEINFO:
			ethtool_struct_size = sizeof(struct ethtool_modinfo);
			break;

		case ETHTOOL_GEEE:
		case ETHTOOL_SEEE:
			ethtool_struct_size = sizeof(struct ethtool_eee);
			break;

		case ETHTOOL_GRSSH:
		case ETHTOOL_SRSSH: {
			/* struct ethtool_rxfh layout:
				+----------------------+------------+----------+
				| sizeof(ethtool_rxfh) | indir_size | key_size |
				+----------------------+------------+----------+

				`indir_size` and `key_size` fields set to full size
				of those fields, not just the number of elements
			*/
			struct ethtool_rxfh *rxfh = (struct ethtool_rxfh *)ethtool_struct;
			ethtool_struct_size = sizeof(struct ethtool_rxfh) + rxfh->indir_size + rxfh->key_size;
			break;
		}

		case ETHTOOL_GTUNABLE:
		case ETHTOOL_STUNABLE:
		case ETHTOOL_PHY_GTUNABLE:
		case ETHTOOL_PHY_STUNABLE: {
			struct ethtool_tunable *tunable = (struct ethtool_tunable *)ethtool_struct;
			ethtool_struct_size = sizeof(struct ethtool_tunable) + tunable->len;
			break;
		}

		case ETHTOOL_PERQUEUE:
			/* sub_command only ever used as ETHTOOL_[G/S]COALESCE in ethtool */
			/* FIXME: maybe use recursion to gather subcmd size? */
			ethtool_struct_size = sizeof(struct ethtool_per_queue_op) + sizeof(struct ethtool_coalesce);
			break;

		case ETHTOOL_GLINKSETTINGS:
		case ETHTOOL_SLINKSETTINGS:
			ethtool_struct_size = ETHTOOL_STRUCT_SIZE_LEN_DATA_NAMES(struct ethtool_link_settings, ethtool_struct, link_mode_masks_nwords, link_mode_masks);
			break;

		case ETHTOOL_GFECPARAM:
		case ETHTOOL_SFECPARAM:
			ethtool_struct_size = sizeof(struct ethtool_fecparam);
			break;

		case ETHTOOL_GMSGLVL:
		case ETHTOOL_SMSGLVL:
		case ETHTOOL_NWAY_RST:
		case ETHTOOL_GLINK:
		case ETHTOOL_GRXCSUM:
		case ETHTOOL_SRXCSUM:
		case ETHTOOL_GTXCSUM:
		case ETHTOOL_STXCSUM:
		case ETHTOOL_GSG:
		case ETHTOOL_SSG:
		case ETHTOOL_PHYS_ID:
		case ETHTOOL_GTSO:
		case ETHTOOL_STSO:
		case ETHTOOL_GUFO:
		case ETHTOOL_SUFO:
		case ETHTOOL_GGSO:
		case ETHTOOL_SGSO:
		case ETHTOOL_GFLAGS:
		case ETHTOOL_SFLAGS:
		case ETHTOOL_GPFLAGS:
		case ETHTOOL_SPFLAGS:
		case ETHTOOL_GGRO:
		case ETHTOOL_SGRO:
		case ETHTOOL_RESET:
			ethtool_struct_size = sizeof(struct ethtool_value);
			break;

		case ETHTOOL_GRXNTUPLE: /* FIXME: not handled by ethtool, should we handle it? */
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
