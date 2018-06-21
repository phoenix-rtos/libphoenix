/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * netinet/in.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
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
	unsigned char s6_addr[16];
};


struct sockaddr_in {
	sa_family_t     sin_family;
	in_port_t       sin_port;
	struct in_addr  sin_addr;
};

struct sockaddr_in6 {
	sa_family_t     sin6_family;
	in_port_t       sin6_port;
	uint32_t        sin6_flowinfo;
	struct in6_addr sin6_addr;
	uint32_t        sin6_scope_id;
};


enum { INADDR_ANY, INADDR_BROADCAST };


#endif
