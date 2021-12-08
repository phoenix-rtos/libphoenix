/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * ip.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Kamil Amanowicz
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_NETINET_IP_H_
#define _LIBPHOENIX_NETINET_IP_H_

#include <arch.h>
#include <stdint.h>


struct iphdr {
#if __BYTE_ORDER == __LITTLE_ENDIAN
	uint8_t ihl:4;     /* Header length */
	uint8_t version:4; /* Version */
#elif __BYTE_ORDER == __BIG_ENDIAN
	uint8_t version:4; /* Version */
	uint8_t ihl:4;     /* Header length */
#else
#error "No byteorder"
#endif
	uint8_t tos;      /* Type of service */
	uint16_t tot_len; /* Total length */
	uint16_t id;      /* ID */
	uint16_t off;     /* Fragment offset */
	uint8_t ttl;      /* Time to live */
	uint8_t proto;    /* Protocol */
	uint16_t chksum;  /* Checksum */
	uint32_t srcaddr; /* Source */
	uint32_t dstaddr; /* Destination */
};


#endif
