#ifndef _NETINET_IP_H
#define _NETINET_IP_H

#include <stdint.h>
#include <netinet/in.h>

struct iphdr {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	uint8_t ihl : 4,     /* header length */
			version : 4; /* version */
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	uint8_t version : 4,
			ihl : 4;
#else
#error "Unsupported byte order"
#endif
	uint8_t tos;      /* type of service */
	uint16_t tot_len; /* total length */
	uint16_t id;      /* id */
	uint16_t off;     /* fragment offset */
	uint8_t ttl;      /* time to live */
	uint8_t proto;    /* protocol */
	uint16_t chksum;  /* checksum */
	uint32_t srcaddr; /* source */
	uint32_t dstaddr; /* destination */
};

struct ip6_hdr {
	uint32_t ver_tc_fl;   /* version, traffic class, flow label */
	uint16_t paylen;      /* payload length */
	uint8_t nexthdr;      /* next header */
	uint8_t hoplimit;     /* hop limit */
	struct in6_addr src;  /* source address */
	struct in6_addr dest; /* destination address */
};

#endif
