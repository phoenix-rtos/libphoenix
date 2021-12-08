/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * ifaddrs.h
 *
 * Copyright 2021 Phoenix Systems
 * Author: Maciej Purski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_IFADDRS_H_
#define _LIBPHOENIX_IFADDRS_H_

#include <sys/socket.h>


struct ifaddrs {
	struct ifaddrs *ifa_next;     /* Next structure */
	char *ifa_name;               /* Network interface name */
	unsigned int ifa_flags;       /* Flags as from SIOCGIFFLAGS ioctl */
	struct sockaddr *ifa_addr;    /* Interface network address */
	struct sockaddr *ifa_netmask; /* Interface network mask */
	struct sockaddr *ifa_dstaddr; /* Point to point destination address */
	void *ifa_data;               /* Address specific data */
};


extern int getifaddrs(struct ifaddrs **);


extern void freeifaddrs(struct ifaddrs *);


#endif /* _LIBPHOENIX_IFADDRS_H_ */
