/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * net/if.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Marek Białowąs
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_NET_IF_H_
#define _LIBPHOENIX_NET_IF_H_

#include <phoenix/posix/sockif.h>


/* Interface flags */
#define IFF_UP          0x0001 /* Interface is up */
#define IFF_BROADCAST   0x0002 /* Broadcast address valid */
#define IFF_DEBUG       0x0004 /* Turn on debugging */
#define IFF_LOOPBACK    0x0008 /* Is a loopback net */
#define IFF_POINTOPOINT 0x0010 /* Interface is point-to-point link */
#define IFF_NOTRAILERS  0x0020 /* Avoid use of trailers */
#define IFF_RUNNING     0x0040 /* Resources allocated */
#define IFF_NOARP       0x0080 /* No address resolution protocol */
#define IFF_PROMISC     0x0100 /* Receive all packets */
#define IFF_ALLMULTI    0x0200 /* Receive all multicast packets */
#define IFF_OACTIVE     0x0400 /* Transmission in progress */
#define IFF_SIMPLEX     0x0800 /* Can't hear own transmissions */
#define IFF_LINK0       0x1000 /* Per link layer defined bit */
#define IFF_LINK1       0x2000 /* Per link layer defined bit */
#define IFF_MULTICAST   0x4000 /* Supports multicast */
#define IFF_DYNAMIC     0x8000 /* The addresses are lost when the interface goes down */


/* Unused but needed by ifconfig */
#define IFF_MASTER IFF_LINK0
#define IFF_SLAVE  IFF_LINK1


struct if_nameindex {
	unsigned int if_index;
	char *if_name;
};


extern unsigned int if_nametoindex(const char *ifname);


extern char *if_indextoname(unsigned int ifindex, char *ifname);


extern struct if_nameindex *if_nameindex(void);


extern void if_freenameindex(struct if_nameindex *ptr);


#endif
