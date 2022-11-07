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


uint16_t ntohs(uint16_t netshort)
{
	return be16toh(netshort);
}
