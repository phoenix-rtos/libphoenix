#ifndef _NETINET_IP_H
#define _NETINET_IP_H

#include <arch.h>

struct iphdr {
/* TODO: fix byteorder */
#if 1 /* defined(__LITTLE_ENDIAN) */
	u8	ihl:4,	/* header length */
		version:4; /* version */
#elif defined (__BIG_ENDIAN)
	u8	version:4,
		ihl:4;
#else
#error	"No byteorder"
#endif
	u8	tos;		/* type of service */
	u16	tot_len;	/* total length */
	u16	id;			/* id */
	u16	off;		/* fragment offset */
	u8	ttl;		/* time to live */
	u8	proto;		/* protocol */
	u16	chksum;		/* checksum */
	u32	srcaddr;	/* source */
	u32	dstaddr;	/* destination */
};

#endif