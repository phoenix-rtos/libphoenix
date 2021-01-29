/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * net/inet_pton.c
 *
 * Copyright 2021 Phoenix Systems
 * Author: Maciej Purski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* Accept only addresses in dotted-decimal format ddd.ddd.ddd.ddd */
static int inet_pton4(const char *src, struct in_addr *dst)
{
	in_addr_t addr4 = 0;
	unsigned long val;
	int i;
	char *endptr;

	for (i = 0; i < 4; i++) {
		if (!isdigit(*src))
			return 0;

		val = strtoul(src, &endptr, 10);
		if (endptr == src || (endptr - src) > 3 || val > 255)
			return 0;
		if ((*endptr != '.' && i < 3) || (*endptr != '\0' && i == 3))
			return 0;

		src = endptr + 1;
		addr4 = addr4 << 8 | val;
	}

	dst->s_addr = htonl(addr4);

	return 1;
}

/*
 * Accepts IPv6 addresses with optional contigous zeros abbreviation
 * or IPv6-mapped IPv4 addresses.
 */
static int inet_pton6(const char *src, struct in6_addr *dst)
{
	struct in6_addr addr6;
	struct in_addr addr4;
	const int n_hextets = sizeof(addr6.s6_addr16) / sizeof(addr6.s6_addr16[0]);
	int i, idx = 0, z = -1;
	unsigned long val;
	const char *endptr, *ptr = src;

	memset(&addr6, 0, sizeof(addr6));
	addr4.s_addr = 0;

	while (*ptr != '\0') {
		if (idx > n_hextets - 1)
			return 0;

		endptr = ptr;
		while (isxdigit(*endptr))
			endptr++;

		if (ptr == endptr && *endptr == ':') {
			/* '::' zeros abbreviation */
			if (z != -1)
				return 0;
			z = idx;
			if (ptr != src)
				ptr = endptr + 1;
			else if (ptr == src && *(endptr + 1) == ':')
				ptr = endptr + 2;
			else
				return 0;
		}
		else if (*endptr == ':' || *endptr == '\0') {
			/* Hexadecimal part */
			if (endptr - ptr > 4)
				return 0;

			val = strtoul(ptr, NULL, 16);
			if (val > 0xFFFF)
				return 0;
			addr6.s6_addr16[idx++] = htons((uint16_t) val);

			if (*endptr == ':')
				ptr = endptr + 1;
			else
				ptr = endptr;
		}
		else if (*endptr == '.') {
			/* IPv6-mapped IPv4 address */
			if (inet_pton4(ptr, &addr4) != 1)
				return 0;
			memcpy(addr6.s6_addr16 + idx, &addr4.s_addr, sizeof(addr4));
			idx += 2;
			break;
		}
		else {
			return 0;
		}
	}

	/* Address is not full */
	if (idx < n_hextets && z == -1)
		return 0;

	/* Address too long */
	if (idx == n_hextets && z != -1)
		return 0;

	/* Copy address and fill the zeros string */
	for (i = 0; i < n_hextets; i++) {
		if (i >= z && i < n_hextets - idx + z)
			dst->s6_addr16[i] = 0;
		else if (i < n_hextets - idx + z)
			dst->s6_addr16[i] = addr6.s6_addr16[i];
		else
			dst->s6_addr16[i] = addr6.s6_addr16[i - (n_hextets - idx)];
	}

	return 1;
}

int inet_pton(int af, const char *src, void *dst)
{
	if (src == NULL || dst == NULL)
		return 0;

	if (af == AF_INET)
		return inet_pton4(src, dst);
	else if (af == AF_INET6)
		return inet_pton6(src, dst);

	errno = EAFNOSUPPORT;

	return -1;
}
