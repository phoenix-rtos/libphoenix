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


extern int inet_aton(const char *cp, struct in_addr *inp);


extern in_addr_t inet_addr(const char *cp);


extern char *inet_ntoa(struct in_addr in);


extern const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);


extern int inet_pton(int af, const char *src, void *dst);


#define htons htobe16
#define ntohs be16toh
#define htonl htobe32
#define ntohl be32toh


#endif
