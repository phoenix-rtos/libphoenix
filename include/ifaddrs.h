/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * net/ifaddrs.h
 *
 * Copyright 2021 Phoenix Systems
 * Author: Maciej Purski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef	_LIBPHOENIX_IFADDRS_H_
#define	_LIBPHOENIX_IFADDRS_H_

struct ifaddrs {
	struct ifaddrs *ifa_next;
	char *ifa_name;
	unsigned int ifa_flags;
	struct sockaddr *ifa_addr;
	struct sockaddr *ifa_netmask;
	struct sockaddr *ifa_dstaddr;
	void *ifa_data;
};

int getifaddrs(struct ifaddrs **);
void freeifaddrs(struct ifaddrs *);

#endif /* _LIBPHOENIX_IFADDRS_H_ */
