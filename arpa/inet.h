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

#define ntohs be16toh
#define ntohl be32toh
#define htons htobe16
#define htonl htobe32

#endif
