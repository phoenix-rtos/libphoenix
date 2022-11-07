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

#include <stdio.h>
#include <arpa/inet.h>

static char static_buffer[18];

char *inet_ntoa(struct in_addr in)
{
	unsigned char *bytes = (unsigned char *) &in.s_addr;
	snprintf(static_buffer, sizeof(static_buffer), "%u.%u.%u.%u", bytes[0], bytes[1], bytes[2], bytes[3]);
	return static_buffer;
}
