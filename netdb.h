/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * netdb.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski, Michał Mirosław
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_NETDB_H_
#define _LIBPHOENIX_NETDB_H_

#include <sys/types.h>


struct sockaddr;


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
	RESOLVED_OK,
	HOST_NOT_FOUND,
	TRY_AGAIN,
	NO_RECOVERY,
	NO_DATA,
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

	NI_MAXHOST = 1025,
	NI_MAXSERV = 32,
};


enum {
	EAI_BADFLAGS = -1,
	EAI_NONAME = -2,
	EAI_AGAIN = -3,
	EAI_FAIL = -4,
	EAI_FAMILY = -6,
	EAI_SOCKTYPE = -7,
	EAI_SERVICE = -8,
	EAI_MEMORY = -10,
	EAI_SYSTEM = -11,
	EAI_OVERFLOW = -12,
};


struct servent {
	char  *s_name;
	char **s_aliases;
	int    s_port;
	char  *s_proto;
};


extern int h_errno;


struct servent *getservbyname(const char *name, const char *proto);
struct servent *getservbyport(int port, const char *proto);


int getnameinfo(const struct sockaddr *addr, socklen_t addrlen,
                char *host, socklen_t hostlen,
                char *serv, socklen_t servlen, int flags);
int getaddrinfo(const char *node, const char *service,
                const struct addrinfo *hints,
                struct addrinfo **res);
void freeaddrinfo(struct addrinfo *res);
const char *gai_strerror(int errcode);


#endif
