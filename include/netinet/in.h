/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * netinet/in.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski, Michal Miroslaw, Kamil Amanowicz
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _NETINET_IN_H_
#define _NETINET_IN_H_

#include <sys/socket.h>
#include <stdint.h>
#include <netinet/intypes.h>


struct in_addr {
	in_addr_t s_addr;
};

struct in6_addr {
	union {
		uint8_t s6_addr[16];
		uint16_t s6_addr16[8];
		uint32_t s6_addr32[4];
	};
};


struct sockaddr_in {
	sa_family_t     sin_family;
	in_port_t       sin_port;
	struct in_addr  sin_addr;
#define SIN_ZERO_LEN 8
	uint8_t		sin_zero[SIN_ZERO_LEN];
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

extern const struct in6_addr in6addr_any;
extern const struct in6_addr in6addr_loopback;

#define IN6ADDR_ANY_INIT  {{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}}
#define IN6ADDR_LOOPBACK_INIT {{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,0, 0, 0, 1}}}

#define INET_ADDRSTRLEN 16 /* Length of the string form for IPv4 */
#define INET6_ADDRSTRLEN 46 /* Length of the string form for IPv6 */

#define IPV6_JOIN_GROUP			0x00000001 /* Join a multicast group */
#define IPV6_LEAVE_GROUP		0x00000002 /* Quit a multicast group */
#define IPV6_MULTICAST_HOPS		0x00000004 /* Multicast hop limit */
#define IPV6_MULTICAST_IF		0x00000008 /* Interface to use for outgoing multicast packets */
#define IPV6_MULTICAST_LOOP		0x00000010 /* Multicast packets are delivered back to the local application */
#define IPV6_UNICAST_HOPS		0x00000020 /* Unicast hop limit */
#define IPV6_V6ONLY				0x00000040 /* Restrict AF_INET6 socket to IPv6 communications only */

/* IPv6 multicast address scopes */
#define IPv6_ADDR_MC_SCOPE_NODELOCAL	0x1 /* Interface-Local scope */
#define IPv6_ADDR_MC_SCOPE_LINKLOCAL	0x2 /* Link-Local scope */
#define IPv6_ADDR_MC_SCOPE_REALMLOCAL	0x3 /* Realm-Local scope */
#define IPv6_ADDR_MC_SCOPE_ADMINLOCAL	0x4 /* Admin-Local scope */
#define IPv6_ADDR_MC_SCOPE_SITELOCAL	0x5 /* Site-Local scope */
#define IPv6_ADDR_MC_SCOPE_ORGLOCAL		0x8 /* Organization-Local scope */
#define IPv6_ADDR_MC_SCOPE_GLOBAL		0xe /* Global scope */

/* Unspecified address */
#define IN6_IS_ADDR_UNSPECIFIED(a) \
	(((a)->s6_addr32[0] == 0 && (a)->s6_addr32[1] == 0 && \
	  (a)->s6_addr32[2] == 0 && (a)->s6_addr32[3] == 0))

/* Loopback address */
#define IN6_IS_ADDR_LOOPBACK(a) \
	(((a)->s6_addr32[0] == 0 && (a)->s6_addr32[1] == 0 && \
	  (a)->s6_addr32[2] == 0 && (a)->s6_addr32[3] == 0x01000000))

/* Multicast address */
#define IN6_IS_ADDR_MULTICAST(a) \
		(((a)->s6_addr[0] == 0xff))

/* Unicast link-local address */
#define IN6_IS_ADDR_LINKLOCAL(a) \
		(((a)->s6_addr[0] == 0xfe && (a)->s6_addr[1] == 0x80))

/* Unicast site-local address */
#define IN6_IS_ADDR_SITELOCAL(a) \
		(((a)->s6_addr[0] == 0xfe && (a)->s6_addr[1] == 0xc0))

/* IPv4 mapped address */
#define IN6_IS_ADDR_V4MAPPED(a) \
		(((a)->s6_addr32[0] == 0 && (a)->s6_addr32[1] == 0 && \
		  (a)->s6_addr32[2] == 0xffff0000))

/* IPv4-compatible address */
#define IN6_IS_ADDR_V4COMPAT(a) \
	(((a)->s6_addr32[0] == 0 && (a)->s6_addr32[1] == 0 && \
	  (a)->s6_addr32[2] == 0 && (a)->s6_addr32[3] != 0x01000000 && \
	  (a)->s6_addr32[3] != 0))

/* Multicast node-local address */
#define IN6_IS_ADDR_MC_NODELOCAL(a) \
		((IN6_IS_ADDR_MULTICAST(a) && \
		  a->s6_addr[1] == IPv6_ADDR_MC_SCOPE_NODELOCAL))

/* Multicast link-local address */
#define IN6_IS_ADDR_MC_LINKLOCAL(a) \
		((IN6_IS_ADDR_MULTICAST(a) && \
		  a->s6_addr[1] == IPv6_ADDR_MC_SCOPE_LINKLOCAL))

/* Multicast site-local address */
#define IN6_IS_ADDR_MC_SITELOCAL(a) \
		((IN6_IS_ADDR_MULTICAST(a) && \
		  a->s6_addr[1] == IPv6_ADDR_MC_SCOPE_SITELOCAL))

/* Multicast organization-local address */
#define IN6_IS_ADDR_MC_ORGLOCAL(a) \
		((IN6_IS_ADDR_MULTICAST(a) && \
		  a->s6_addr[1] == IPv6_ADDR_MC_SCOPE_ORGLOCAL))

/* Multicast global address */
#define IN6_IS_ADDR_MC_GLOBAL(a) \
		((IN6_IS_ADDR_MULTICAST(a) && \
		  a->s6_addr[1] == IPv6_ADDR_MC_SCOPE_GLOBAL))

/* Not defined by POSIX but still needed */
#define IN6_ARE_ADDR_EQUAL(a, b) \
	(((a)->s6_addr32[0] == (b)->s6_addr32[0] && \
	  (a)->s6_addr32[1] == (b)->s6_addr32[1] && \
	  (a)->s6_addr32[2] == (b)->s6_addr32[2] && \
	  (a)->s6_addr32[3] == (b)->s6_addr32[3]))

enum {
	IP_TOS = 1,
	IP_TTL = 2,
	IP_MULTICAST_TTL = 5,
	IP_MULTICAST_IF = 6,
	IP_IPSEC_POLICY = 16 /* Set/get security policy */
};

enum {
	IPPORT_RESERVED = 0,
	IPPORT_USERRESERVED = 4096
};


#endif /* _NETINET_IN_H_ */
