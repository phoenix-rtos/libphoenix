/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * net/ip6_route.h
 *
 * Copyright 2024 Phoenix Systems
 * Author: Mateusz Kobak
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */


#ifndef _LIBPHOENIX_NET_IP6_ROUTE_H
#define _LIBPHOENIX_NET_IP6_ROUTE_H

#include <netinet/in.h>

/*
 * This is needed to compile route.c in busybox.
 * Routes for IPv6 are not implemented yet
 */
struct in6_rtmsg {
	struct in6_addr rtmsg_dst;
	struct in6_addr rtmsg_src;
	struct in6_addr rtmsg_gateway;
	uint32_t rtmsg_type;
	uint16_t rtmsg_dst_len;
	uint16_t rtmsg_src_len;
	uint32_t rtmsg_metric;
	unsigned long int rtmsg_info;
	uint32_t rtmsg_flags;
	int rtmsg_ifindex;
};


#endif /* _LIBPHOENIX_NET_IP6_ROUTE_H */
