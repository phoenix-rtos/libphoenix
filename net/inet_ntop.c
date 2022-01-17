/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * net/inet_ntop.c
 *
 * Copyright 2021 Phoenix Systems
 * Author: Maciej Purski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>


static const char *inet_ntop4(const void *src, char *dst, socklen_t size)
{
	if (size < INET_ADDRSTRLEN) {
		errno = ENOSPC;
		return NULL;
	}

	snprintf(dst, size, "%u.%u.%u.%u",
	        (unsigned int)((unsigned char *)src)[0],
	        (unsigned int)((unsigned char *)src)[1],
	        (unsigned int)((unsigned char *)src)[2],
	        (unsigned int)((unsigned char *)src)[3]);

	return dst;
}

static const char *inet_ntop6(const void *src, char *dst, socklen_t size)
{
	const struct in6_addr *addr;
	const int n_hextets = sizeof(addr->s6_addr16) / sizeof(addr->s6_addr16[0]);
	int i, zero_idx = -1, tmp_len = 0, zero_len = 0, tmp_idx = -1;
	char *dst_ptr = dst;

	if (size < INET6_ADDRSTRLEN) {
		errno = ENOSPC;
		return NULL;
	}
	memset(dst, 0, size);

	addr = (const struct in6_addr *) src;
	if (IN6_IS_ADDR_V4MAPPED(addr) || IN6_IS_ADDR_V4COMPAT(addr)) {
		snprintf(dst, INET6_ADDRSTRLEN, "::%s%u.%u.%u.%u",
		         addr->s6_addr16[5] == 0xFFFF ? "ffff:" : "",
		         addr->s6_addr[12],
		         addr->s6_addr[13],
		         addr->s6_addr[14],
		         addr->s6_addr[15]);
		return dst;
	}

	/* Find the longest all-zero string */
	for (i = 0; i < n_hextets; i++) {
		if (addr->s6_addr16[i] == 0) {
			if (tmp_idx == -1) {
				/* Found new all-zero string */
				tmp_idx = i;
				tmp_len = 1;
			}
			else {
				tmp_len++;
			}
		}
		/* Check if, all-zero string is over */
		if (tmp_idx != -1 && (addr->s6_addr16[i] != 0 || i == n_hextets - 1)) {
			if (tmp_len > 1 && tmp_len > zero_len) {
				zero_len = tmp_len;
				zero_idx = tmp_idx;
			}
			tmp_idx = -1;
		}
	}

	i = 0;
	/* Leading zeros */
	if (zero_idx == 0) {
		*(dst_ptr++) = ':';
		i += zero_len;
	}

	while (i < n_hextets) {
		if (i != 0)
			*(dst_ptr++) = ':';
		/* Zeros inside */
		if (i == zero_idx) {
			i += zero_len;
		} else {
			dst_ptr += snprintf(dst_ptr, 5, "%x", ntohs(addr->s6_addr16[i]));
			i++;
		}
	}

	/* Trailing zeros */
	if (zero_idx + zero_len == n_hextets)
		*(dst_ptr++) = ':';

	return dst;
}

const char *inet_ntop(int af, const void *src,
                      char *dst, socklen_t size)
{
	if (dst == NULL || src == NULL)
		return NULL;

	if (af == AF_INET)
		return inet_ntop4(src, dst, size);
	else if (af == AF_INET6)
		return inet_ntop6(src, dst, size);

	errno = EAFNOSUPPORT;

	return NULL;
}
