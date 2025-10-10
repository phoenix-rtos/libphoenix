/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * net/if6.h
 *
 * Copyright 2021 Phoenix Systems
 * Author: Maciej Purski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_NET_IF6_H
#define _LIBPHOENIX_NET_IF6_H

#include <sys/types.h>


struct in6_addrlifetime {
	time_t expire;
	time_t preferred;
};

struct in6_ifreq {
	union {
		char ifrn_name[IFNAMSIZ];
	} ifr_ifrn;

	union {
		struct sockaddr_in6 ifru_addr;
		struct sockaddr_in6 ifru_dstaddr;
		int ifru_flags6;
		int ifru_metric;
		caddr_t ifru_data;
		struct in6_addrlifetime ifru_lifetime;
	} ifr_ifru;
};

#define ifr_name6     ifr_ifrn.ifrn_name     /* Interface name */
#define ifr_addr6     ifr_ifru.ifru_addr     /* Address */
#define ifr_dstaddr6  ifr_ifru.ifru_dstaddr  /* Other end of p-p lnk */
#define ifr_flags6    ifr_ifru.ifru_flags6   /* Flags */
#define ifr_metric6   ifr_ifru.ifru_metric   /* Metric */
#define ifr_data6     ifr_ifru.ifru_data     /* For use by interface */
#define ifr_lifetime6 ifr_ifru.ifru_lifetime /* Address validity time */

struct in6_aliasreq {
	char ifra_name[IFNAMSIZ];
	struct sockaddr_in6 ifrau_addr;
	struct sockaddr_in6 ifra_dstaddr;
	struct sockaddr_in6 ifra_prefixmask;
	int ifra_flags;
	struct in6_addrlifetime ifra_lifetime;
};


/* lwIP configuration */
#define LWIP_IPV6_NUM_ADDRESSES 3

/* IPv6 ioctls */
#define SIOCGIFADDR_IN6      _IOWR(SOCK_IOC_TYPE, 0x21, struct in6_ifreq[LWIP_IPV6_NUM_ADDRESSES])
#define SIOCGIFDSTADDR_IN6   _IOWR(SOCK_IOC_TYPE, 0x22, struct in6_ifreq)
#define SIOCGIFNETMASK_IN6   _IOWR(SOCK_IOC_TYPE, 0x25, struct in6_ifreq)
#define SIOCDIFADDR_IN6      _IOW(SOCK_IOC_TYPE, 0x19, struct in6_ifreq)
#define SIOCAIFADDR_IN6      _IOW(SOCK_IOC_TYPE, 0x1a, struct in6_aliasreq)
#define SIOCGIFAFLAG_IN6     _IOWR(SOCK_IOC_TYPE, 0x49, struct in6_ifreq)
#define SIOCGIFALIFETIME_IN6 _IOWR(SOCK_IOC_TYPE, 0x51, struct in6_ifreq)

/* Address flags, fetched using SIOCGIFAFLAG_IN6 */
#define IN6_IFF_ANYCAST    0x01
#define IN6_IFF_TENTATIVE  0x02
#define IN6_IFF_DUPLICATED 0x04
#define IN6_IFF_DETACHED   0x08
#define IN6_IFF_DEPRECATED 0x10
#define IN6_IFF_AUTOCONF   0x20

#endif /* _LIBPHOENIX_NET_IF6_H */
