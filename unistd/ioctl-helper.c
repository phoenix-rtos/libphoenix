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
#include <sys/socket.h>
#include <phoenix/ethtool.h>

#include "ioctl-helper.h"

#define PTR_ADD(ptr, offset)           ((void *)(((uint8_t *)(ptr)) + (offset)))
#define PTR_ADD_AND_DEREF(ptr, offset) (*(void **)PTR_ADD(ptr, offset))

#define IS_SPECIAL_IOCTL(request) ( \
		(request == SIOCGIFCONF) || \
		(request == SIOCADDRT) || \
		(request == SIOCDELRT) || \
		(request == SIOCETHTOOL))


typedef struct {
	size_t offset;
	size_t size;
} subptr_info_t;

typedef int (*ioctl_op_t)(void **, size_t *, void *, size_t, const subptr_info_t *, size_t);


static inline size_t ioctl_totalSize(size_t ioctl_val_size, const subptr_info_t *subptrs, size_t nsubptrs)
{
	size_t total_size = ioctl_val_size;

	for (size_t i = 0; i < nsubptrs; i++) {
		total_size += subptrs[i].size;
	}

	return total_size;
}


static int ioctl_packStructWithSubptrs(void **packed, size_t *packed_size, void *ioctl_val, size_t ioctl_val_size, const subptr_info_t *subptrs, size_t nsubptrs)
{
	if (nsubptrs == 0 || subptrs == NULL) {
		/* nothing to pack */
		*packed = ioctl_val;
		*packed_size = ioctl_val_size;
		return 0;
	}

	const size_t total_size = ioctl_totalSize(ioctl_val_size, subptrs, nsubptrs);
	size_t offset = 0;

	void *mem = malloc(total_size);
	if (mem == NULL) {
		return -ENOMEM;
	}

	(void)memcpy(PTR_ADD(mem, offset), ioctl_val, ioctl_val_size);
	offset += ioctl_val_size;

	for (size_t i = 0; i < nsubptrs; i++) {
		(void)memcpy(PTR_ADD(mem, offset), PTR_ADD_AND_DEREF(ioctl_val, subptrs[i].offset), subptrs[i].size);
		offset += subptrs[i].size;
	}

	*packed = mem;
	*packed_size = total_size;

	return 0;
}


static int ioctl_unpackStructWithSubptrs(void **packed, size_t *packed_size, void *ioctl_val, size_t ioctl_val_size, const subptr_info_t *subptrs, size_t nsubptrs)
{
	(void)packed_size;
	void *from = *packed;
	size_t offset = ioctl_val_size;

	if (nsubptrs == 0 || subptrs == NULL) {
		/* nothing to unpack */
		(void)memcpy(ioctl_val, from, ioctl_val_size);
		return 0;
	}

	for (size_t i = 0; i < nsubptrs; i++) {
		/* copy old pointer value to helper variable */
		void *subptr = PTR_ADD_AND_DEREF(ioctl_val, subptrs[i].offset);
		/* set pointer value in allocated structure to copy the right pointer at the end */
		PTR_ADD_AND_DEREF(from, subptrs[i].offset) = subptr;

		/* copy current substructure */
		(void)memcpy(subptr, PTR_ADD(from, offset), subptrs[i].size);
		offset += subptrs[i].size;
	}

	/* copy back the main struct */
	(void)memcpy(ioctl_val, from, ioctl_val_size);

	return 0;
}


static inline int ioctl_ethtool(struct ifreq *ifr, void **packed, size_t *packed_size, ioctl_op_t op)
{
	size_t ethtool_size;
	void *ethtool = ifr->ifr_data;

	if (ethtool == NULL) {
		return -EINVAL;
	}

	/* first field of every ethtool struct is always uint32_t cmd */
	uint32_t cmd = *((uint32_t *)ethtool);

	switch (cmd) {
		case ETHTOOL_GSET:
		case ETHTOOL_SSET:
			ethtool_size = sizeof(struct ethtool_cmd);
			break;

		case ETHTOOL_TEST: {
			struct ethtool_test *test = ethtool;
			ethtool_size = sizeof(*test) + (test->len * sizeof(test->data[0]));
			break;
		}

		case ETHTOOL_GSTRINGS: {
			struct ethtool_gstrings *gstrings = ethtool;
			ethtool_size = sizeof(*gstrings) + (gstrings->len * ETH_GSTRING_LEN);
			break;
		}

		case ETHTOOL_GSSET_INFO: {
			struct ethtool_sset_info *sset = ethtool;
			ethtool_size = sizeof(*sset) + (__builtin_popcountll(sset->sset_mask) * sizeof(sset->data[0]));
			break;
		}

		case ETHTOOL_GLOOPBACK:
		case ETHTOOL_SLOOPBACK:
			ethtool_size = sizeof(struct ethtool_value);
			break;

		case ETHTOOL_GLINKSETTINGS:
		case ETHTOOL_SLINKSETTINGS: {
			struct ethtool_link_settings *settings = ethtool;
			ethtool_size = sizeof(*settings) + (settings->link_mode_masks_nwords * sizeof(settings->link_mode_masks[0]));
			break;
		}

		default:
			return -EOPNOTSUPP;
	}

	const subptr_info_t info = { .size = ethtool_size, .offset = __builtin_offsetof(struct ifreq, ifr_data) };
	return op(packed, packed_size, ifr, sizeof(*ifr), &info, 1);
}


static inline int ioctl_rtEntry(struct rtentry *rt, void **packed, size_t *packed_size, ioctl_op_t op)
{
	size_t rt_dev_size = 0;
	if (rt->rt_dev == NULL) {
		return -EINVAL;
	}
	rt_dev_size = strlen(rt->rt_dev);

	const subptr_info_t info = { .size = rt_dev_size, .offset = __builtin_offsetof(struct rtentry, rt_dev) };
	return op(packed, packed_size, rt, sizeof(*rt), &info, 1);
}


static inline int ioctl_ifconf(struct ifconf *ifc, void **packed, size_t *packed_size, ioctl_op_t op)
{
	size_t ifc_buf_len = ifc->ifc_len;
	if (ifc->ifc_buf == NULL) {
		if (ifc_buf_len == 0) {
			/* device returns the needed length
			   -> no packing/unpacking needed */
			return op(packed, packed_size, ifc, sizeof(*ifc), NULL, 0);
		}
		else {
			return -EINVAL;
		}
	}

	const subptr_info_t info = { .size = ifc_buf_len, .offset = __builtin_offsetof(struct ifconf, ifc_buf) };
	return op(packed, packed_size, ifc, sizeof(*ifc), &info, 1);
}


void ioctl_free(unsigned long request, void *packed)
{
	if (request == SIOCGIFCONF) {
		struct ifconf *ifc = packed;
		if (ifc->ifc_buf == NULL) {
			/* special case - value was not packed */
			return;
		}
	}
	if (IS_SPECIAL_IOCTL(request)) {
		if (packed != NULL) {
			free(packed);
		}
	}
}


int ioctl_serialize(unsigned long request, void *ioctl_val, void **packed, size_t *packed_size)
{
	int ret = 0;

	/* set to NULL so if we don't allocate the memory, we won't call free on it */
	*packed = NULL;

	switch (request) {
		case SIOCADDRT:
		case SIOCDELRT:
			ret = ioctl_rtEntry(ioctl_val, packed, packed_size, ioctl_packStructWithSubptrs);
			break;

		case SIOCGIFCONF:
			ret = ioctl_ifconf(ioctl_val, packed, packed_size, ioctl_packStructWithSubptrs);
			break;

		case SIOCETHTOOL:
			ret = ioctl_ethtool(ioctl_val, packed, packed_size, ioctl_packStructWithSubptrs);
			break;

		default:
			*packed = ioctl_val;
			*packed_size = IOCPARM_LEN(request);
			ret = 0;
			break;
	}
	if (ret < 0) {
		ioctl_free(request, *packed);
	}

	return ret;
}


void ioctl_deserialize(unsigned long request, void *ioctl_val, void *packed)
{
	switch (request) {
		case SIOCADDRT:
		case SIOCDELRT:
			(void)ioctl_rtEntry(ioctl_val, &packed, NULL, ioctl_unpackStructWithSubptrs);
			break;

		case SIOCGIFCONF:
			(void)ioctl_ifconf(ioctl_val, &packed, NULL, ioctl_unpackStructWithSubptrs);
			break;

		case SIOCETHTOOL:
			(void)ioctl_ethtool(ioctl_val, &packed, NULL, ioctl_unpackStructWithSubptrs);
			break;

		default:
			/* nothing to do */
			return;
	}

	ioctl_free(request, packed);
}
