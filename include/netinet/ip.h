#ifndef _NETINET_IP_H
#define _NETINET_IP_H

#include <stdint.h>

struct iphdr {
/* TODO: fix byteorder */
#if 1 /* defined(__LITTLE_ENDIAN) */
	uint8_t ihl:4,	/* header length */
		version:4; /* version */
#elif defined (__BIG_ENDIAN)
	uint8_t version:4,
		ihl:4;
#else
#error	"No byteorder"
#endif
	uint8_t tos;		/* type of service */
	uint16_t tot_len;	/* total length */
	uint16_t id;		/* id */
	uint16_t off;		/* fragment offset */
	uint8_t ttl;		/* time to live */
	uint8_t proto;		/* protocol */
	uint16_t chksum;	/* checksum */
	uint32_t srcaddr;	/* source */
	uint32_t dstaddr;	/* destination */
};

#endif
