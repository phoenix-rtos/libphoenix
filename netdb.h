/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * netdb.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_NETDB_H_
#define _LIBPHOENIX_NETDB_H_

#include <netinet/in.h>

extern int h_errno;


struct hostent {
	char  *h_name;
	char **h_aliases;
	int    h_addrtype;
	int    h_length;
	char **h_addr_list;
};


struct addrinfo {
	int              ai_flags;
	int              ai_family;
	int              ai_socktype;
	int              ai_protocol;
	socklen_t        ai_addrlen;
	struct sockaddr *ai_addr;
	char            *ai_canonname;
	struct addrinfo *ai_next;
};


enum {
	AI_PASSIVE = 1 << 0,
	AI_CANONNAME = 1 << 1,
	AI_NUMERICHOST = 1 << 2,
	AI_NUMERICSERV = 1 << 3,
	AI_V4MAPPED = 1 << 4,
	AI_ALL = 1 << 5,
	AI_ADDRCONFIG = 1 << 6,
};


enum {
	NI_NOFQDN = 1 << 0,
	NI_NUMERICHOST = 1 << 1,
	NI_NAMEREQD = 1 << 2,
	NI_NUMERICSERV = 1 << 3,
	NI_NUMERICSCOPE = 1 << 4,
	NI_DGRAM = 1 << 5,
};


struct servent {
	char  *s_name;
	char **s_aliases;
	int    s_port;
	char  *s_proto;
};

#endif
