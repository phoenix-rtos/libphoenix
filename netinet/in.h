/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * netinet/in.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski, Michał Mirosław
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _NETINET_IN_H_
#define _NETINET_IN_H_

#include <sys/socket.h>


typedef u16 in_port_t;
typedef u32 in_addr_t;


struct in_addr {
	in_addr_t s_addr;
};

struct in6_addr {
	union {
		u8 s6_addr[16];
		u16 s6_addr16[8];
		u32 s6_addr32[4];
	};
};


struct sockaddr_in {
	sa_family_t     sin_family;
	in_port_t       sin_port;
	struct in_addr  sin_addr;
#define SIN_ZERO_LEN 8
	u8		sin_zero[SIN_ZERO_LEN];
};

struct sockaddr_in6 {
	sa_family_t     sin6_family;
	in_port_t       sin6_port;
	uint32_t        sin6_flowinfo;
	struct in6_addr sin6_addr;
	uint32_t        sin6_scope_id;
};


enum {
	IPPROTO_IP = 0,
	IPPROTO_ICMP = 1,
	IPPROTO_IGMP = 2,
	IPPROTO_IPIP = 4,
	IPPROTO_TCP = 6,
	IPPROTO_UDP = 17,
	IPPROTO_DCCP = 33,
	IPPROTO_IPV6 = 41,
	IPPROTO_GRE = 47,
	IPPROTO_ESP = 50,
	IPPROTO_AH = 51,
	IPPROTO_SCTP = 132,
	IPPROTO_UDPLITE = 136,
	IPPROTO_RAW = 255,
	IPPROTO_MAX = IPPROTO_RAW
};


enum {
	INADDR_ANY = 0,
	INADDR_LOOPBACK = (uint32_t) 0x7f000001,
	INADDR_BROADCAST = ~(uint32_t)0,
	INADDR_NONE = 0xffffffff,
};


enum {
	IP_TOS,
	IP_TTL,
};

#endif /* _NETINET_IN_H_ */
