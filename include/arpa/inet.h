/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * arpa/inet.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Michal Miroslaw
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


#ifdef __cplusplus
extern "C" {
#endif


int inet_aton(const char *cp, struct in_addr *inp);
in_addr_t inet_addr(const char *cp);
char *inet_ntoa(struct in_addr in);
const char *inet_ntop(int af, const void *src,
                      char *dst, socklen_t size);
int inet_pton(int af, const char *src, void *dst);

uint16_t ntohs(uint16_t netshort);
#define ntohl be32toh
#define htons htobe16
#define htonl htobe32


#ifdef __cplusplus
}
#endif


#endif
