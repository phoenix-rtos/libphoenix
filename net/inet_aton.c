/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Internet address manipulation routines
 *
 * Copyright 2018 Phoenix Systems
 * Author: Marek Bialowas
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <arpa/inet.h>
#include <ctype.h>
#include <netinet/in.h>
#include <stdlib.h>

int inet_aton(const char *cp, struct in_addr *inp)
{
	unsigned int parts[4];
	in_addr_t val;
	int parts_cnt = 0;
	char *c = (char*) cp;
	char *endptr;

	while (1) {
		val = strtoul(c, &endptr, 0);
		if (endptr == c)
			return 0;

		parts[parts_cnt] = val;
		c = endptr;

		if (*c == '.') {
			if (parts_cnt >= 3) /* more than 3 '.' */
				return 0;

			parts_cnt += 1;
			c += 1;
		} else if (*c == '\0') {
			break;
		} else if (isspace((unsigned char)*c)) {
			break;
		} else {
			return 0; /* invalid character */
		}
	}

	switch (parts_cnt) {
	case 0:	/* a -- 32 bits */
		break;
	case 1:	/* a.b -- 8.24 bits */
		if (val > 0x00ffffff || parts[0] > 0xff)
			return 0;

		val |= parts[0] << 24;
		break;
	case 2:	/* a.b.c -- 8.8.16 bits */
		if (val > 0x0000ffff || parts[0] > 0xff || parts[1] > 0xff)
			return 0;

		val |= (parts[0] << 24) | (parts[1] << 16);
		break;
	case 3:	/* a.b.c.d -- 8.8.8.8 bits */
		if (val > 0xff || parts[0] > 0xff || parts[1] > 0xff || parts[2] > 0xff)
			return 0;

		val |= (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8);
		break;
	}

	if (inp != NULL)
		inp->s_addr = htonl(val);

	return 1;
}
