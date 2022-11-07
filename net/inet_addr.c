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

in_addr_t inet_addr(const char *cp)
{
	struct in_addr val;

	if (inet_aton(cp, &val))
		return val.s_addr;

	return INADDR_NONE;
}
