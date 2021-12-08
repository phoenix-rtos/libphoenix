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

#ifndef _LIBPHOENIX_NET_IF6_H_
#define _LIBPHOENIX_NET_IF6_H_

#include <net/if.h>
#include <netinet/in.h>
#include <sys/types.h>


/* Address flags, fetched using SIOCGIFAFLAG_IN6 */
#define IN6_IFF_ANYCAST    0x01 /* Anycast address */
#define IN6_IFF_TENTATIVE  0x02 /* Tentative address */
#define IN6_IFF_DUPLICATED 0x04 /* DAD detected duplicate */
#define IN6_IFF_DETACHED   0x08 /* May be detached from the link */
#define IN6_IFF_DEPRECATED 0x10 /* Deprecated address */
#define IN6_IFF_AUTOCONF   0x20 /* Autoconfigurable address */


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
		short ifru_flags;
		int ifru_flags6;
		int ifru_metric;
		caddr_t ifru_data;
		struct in6_addrlifetime ifru_lifetime;
	} ifr_ifru;
};


struct in6_aliasreq {
	char ifra_name[IFNAMSIZ];
	struct sockaddr_in6 ifrau_addr;
	struct sockaddr_in6 ifra_dstaddr;
	struct sockaddr_in6 ifra_prefixmask;
	int ifra_flags;
	struct in6_addrlifetime ifra_lifetime;
};


#endif
