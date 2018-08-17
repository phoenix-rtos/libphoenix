/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * arpa/inet.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Michał Mirosław
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_ARPA_INET_H_
#define _LIBPHOENIX_ARPA_INET_H_

#include <endian.h>
#include <netinet/in.h>
#include <stdint.h>

int inet_aton(const char *cp, struct in_addr *inp);
in_addr_t inet_addr(const char *cp);
char *inet_ntoa(struct in_addr in);

uint16_t ntohs(uint16_t netshort);
#define ntohl be32toh
#define htons htobe16
#define htonl htobe32

#endif
