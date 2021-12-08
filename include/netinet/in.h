/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * netinet/in.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski, Michał Mirosław, Kamil Amanowicz
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_NETINET_IN_H_
#define _LIBPHOENIX_NETINET_IN_H_

#include <inttypes.h>
#include <sys/socket.h>


typedef uint16_t in_port_t;
typedef uint32_t in_addr_t;


/* Address string form buffer sizes */
#define INET_ADDRSTRLEN  16 /* Length of the string form for IPv4 */
#define INET6_ADDRSTRLEN 46 /* Length of the string form for IPv6 */


/* Local addresses */
#define INADDR_ANY       ((in_addr_t)0x00000000) /* Address to accept any incoming messages (0.0.0.0) */
#define INADDR_LOOPBACK  ((in_addr_t)0x7f000001) /* Address to loopback in software to local host (127.0.0.1) */
#define INADDR_BROADCAST ((in_addr_t)0xffffffff) /* Address to send to all hosts (255.255.255.255) */
#define INADDR_NONE      ((in_addr_t)0xffffffff) /* Address indicating an error return (255.255.255.255) */


/* IPv6 multicast address scopes */
#define IPv6_ADDR_MC_SCOPE_NODELOCAL  0x01 /* Interface-Local scope */
#define IPv6_ADDR_MC_SCOPE_LINKLOCAL  0x02 /* Link-Local scope */
#define IPv6_ADDR_MC_SCOPE_REALMLOCAL 0x03 /* Realm-Local scope */
#define IPv6_ADDR_MC_SCOPE_ADMINLOCAL 0x04 /* Admin-Local scope */
#define IPv6_ADDR_MC_SCOPE_SITELOCAL  0x05 /* Site-Local scope */
#define IPv6_ADDR_MC_SCOPE_ORGLOCAL   0x08 /* Organization-Local scope */
#define IPv6_ADDR_MC_SCOPE_GLOBAL     0x0e /* Global scope */


/* Optname argument for getsockopt()/setsockopt() */
#define IPV6_JOIN_GROUP     0x01 /* Join a multicast group */
#define IPV6_LEAVE_GROUP    0x02 /* Quit a multicast group */
#define IPV6_MULTICAST_HOPS 0x04 /* Multicast hop limit */
#define IPV6_MULTICAST_IF   0x08 /* Interface to use for outgoing multicast packets */
#define IPV6_MULTICAST_LOOP 0x10 /* Multicast packets are delivered back to the local application */
#define IPV6_UNICAST_HOPS   0x20 /* Unicast hop limit */
#define IPV6_V6ONLY         0x40 /* Restrict AF_INET6 socket to IPv6 communications only */


/* Level argument for getsockopt()/setsockopt() */
enum {
	IPPROTO_IP      = 0,   /* Internet Protocol */
	IPPROTO_ICMP    = 1,   /* Internet Control Message Protocol */
	IPPROTO_IGMP    = 2,   /* Internet Group Management Protocol */
	IPPROTO_IPIP    = 4,   /* IPIP tunnels */
	IPPROTO_TCP     = 6,   /* Transmission Control Protocol */
	IPPROTO_UDP     = 17,  /* User Datagram Protocol */
	IPPROTO_DCCP    = 33,  /* Datagram Congestion Control Protocol */
	IPPROTO_IPV6    = 41,  /* IPv6 header */
	IPPROTO_GRE     = 47,  /* General Routing Encapsulation */
	IPPROTO_ESP     = 50,  /* Encapsulating Security Payload */
	IPPROTO_AH      = 51,  /* Authentication Header */
	IPPROTO_SCTP    = 132, /* Stream Control Transmission Protocol */
	IPPROTO_UDPLITE = 136, /* UDP-Lite protocol */
	IPPROTO_RAW     = 255, /* Raw IP packets */
	IPPROTO_MAX     = IPPROTO_RAW
};


/* IP level getsockopt()/setsockopt() options */
enum {
	IP_TOS           = 1, /* IP type of service and precedence */
	IP_TTL           = 2, /* IP time to live */
	IP_MULTICAST_TTL = 5, /* Get/set IP multicast ttl */
	IP_MULTICAST_IF  = 6, /* Get/set IP multicast i/f */
	IP_PKTINFO       = 8, /* Get/set incoming packet info */
	IP_IPSEC_POLICY  = 16 /* Get/set security policy */
};


/* Internet address structure */
struct in_addr {
	in_addr_t s_addr;
};


/* Include <arpa/inet.h> for htons(), ntohs(), htonl() and ntohl() */
/* Has to be included after struct in_addr declaration */
#include <arpa/inet.h>


struct sockaddr_in {
	sa_family_t sin_family;   /* AF_INET */
	in_port_t sin_port;       /* Port number in network byte order */
	struct in_addr  sin_addr; /* IP address in network byte order */
#define SIN_ZERO_LEN 8
	uint8_t sin_zero[SIN_ZERO_LEN];
};


/* Stores 128-bit IPv6 address */
struct in6_addr {
	union {
		uint8_t s6_addr[16];
		uint16_t s6_addr16[8];
		uint32_t s6_addr32[4];
	};
};


struct sockaddr_in6 {
	sa_family_t sin6_family;   /* AF_INET6 */
	in_port_t sin6_port;       /* Port number in network byte order */
	uint32_t sin6_flowinfo;    /* IPv6 traffic class and flow information */
	struct in6_addr sin6_addr; /* IPv6 address in network byte order */
	uint32_t sin6_scope_id;    /* Set of interfaces for a scope */
};


struct ipv6_mreq {
	struct in6_addr ipv6mr_multiaddr; /* IPv6 multicast address */
	unsigned int ipv6mr_interface;    /* Interface index */
};


extern const struct in6_addr in6addr_any;      /* Wildcard IPv6 address */
extern const struct in6_addr in6addr_loopback; /* Loopback IPv6 address */


#define IN6ADDR_ANY_INIT      {{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}}
#define IN6ADDR_LOOPBACK_INIT {{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,0, 0, 0, 1}}}


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


#endif
