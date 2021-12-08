/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * netdb.h
 *
 * Copyright 2018, 2021 Phoenix Systems
 * Author: Jan Sikorski, Michał Mirosław, Lukasz Kosinski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_NETDB_H_
#define _LIBPHOENIX_NETDB_H_

#include <inttypes.h>
#include <netinet/in.h>
#include <sys/socket.h>


/* Helper constants */
#define IPPORT_RESERVED     0    /* Ports < IPPORT_RESERVED are reserved for privileged processes */
#define IPPORT_USERRESERVED 4096 /* Ports > IPPORT_USERRESERVED are reserved for servers (not necessarily privileged) */
#define NI_MAXHOST          1025 /* Maximum hostname buffer size in bytes */
#define NI_MAXSERV          32   /* Maximum service name buffer size in bytes */


/* Input flags for struct addrinfo */
#define AI_PASSIVE     (1 << 0) /* Socket address is intended for bind() */
#define AI_CANONNAME   (1 << 1) /* Request for canonical name */
#define AI_NUMERICHOST (1 << 2) /* Return numeric host address as name */
#define AI_NUMERICSERV (1 << 3) /* Inhibit service name resolution */
#define AI_V4MAPPED    (1 << 4) /* If no IPv6 addresses are found, query for IPv4 addresses and return them to the caller as IPv4-mapped IPv6 addresses */
#define AI_ALL         (1 << 5) /* Query for both IPv4 and IPv6 addresses */
#define AI_ADDRCONFIG  (1 << 6) /* Query for IPv4 addresses only when an IPv4 address is configured; query for IPv6 addresses only when an IPv6 address is configured */


/* Flags for getnameinfo() */
#define NI_NOFQDN       (1 << 0) /* Only the nodename portion of the FQDN is returned for local hosts */
#define NI_NUMERICHOST  (1 << 1) /* The numeric form of the node's address is returned instead of its name */
#define NI_NAMEREQD     (1 << 2) /* Return an error if the node's name cannot be located in the database */
#define NI_NUMERICSERV  (1 << 3) /* The numeric form of the service address is returned instead of its name */
#define NI_NUMERICSCOPE (1 << 4) /* For IPv6 addresses, the numeric form of the scope identifier is returned instead of its name */
#define NI_DGRAM        (1 << 5) /* Indicates that the service is a datagram service (SOCK_DGRAM) */


/* Error values for getaddrinfo() and getnameinfo() */
#define EAI_BADFLAGS   -1  /* The flags had an invalid value */
#define EAI_NONAME     -2  /* The name does not resolve for the supplied parameters (NI_NAMEREQD is set and the host's name cannot be located, or both nodename and servname were null) */
#define EAI_AGAIN      -3  /* The name could not be resolved at this time. Future attempts may succeed */
#define EAI_FAIL       -4  /* A non-recoverable error occurred */
#define EAI_NODATA     -5  /* No address associated with given name, non-POSIX */
#define EAI_FAMILY     -6  /* The address family was not recognized or the address length was invalid for the specified family */
#define EAI_SOCKTYPE   -7  /* The intended socket type was not recognized */
#define EAI_SERVICE    -8  /* The service passed was not recognized for the specified socket type */
#define EAI_ADDRFAMILY -9  /* Address family for give name not supported, non-POSIX */
#define EAI_MEMORY     -10 /* There was a memory allocation failure */
#define EAI_SYSTEM     -11 /* A system error occurred. The error code can be found in errno */
#define EAI_OVERFLOW   -12 /* An argument buffer overflowed */


/* DNS API errors */
enum {
	RESOLVED_OK,    /* Success */
	HOST_NOT_FOUND, /* Host not found error */
	TRY_AGAIN,      /* Try again later error */
	NO_RECOVERY,    /* Nonrecoverable name server error */
	NO_DATA         /* Requested name is valid but does not have an IP address*/
};


struct hostent {
	char *h_name;       /* Official name of the host */
	char **h_aliases;   /* Alternative host names */
	int h_addrtype;     /* Address type */
	int h_length;       /* Address length in bytes */
	char **h_addr_list; /* Network addresses for the host (in network byte order) */
};


struct netent
{
	char *n_name;     /* Official name of the host */
	char **n_aliases; /* Alternative network names */
	int n_addrtype;   /* Network address type */
	uint32_t n_net;   /* Network number in host byte order */
};


struct protoent {
	char *p_name;     /* Official name of the protocol */
	char **p_aliases; /* Alternative protocol names */
	int p_proto;      /* Protocol number */
};


struct servent {
	char *s_name;     /* Official name of the service */
	char **s_aliases; /* Alternative service names */
	int s_port;       /* Service port number in network byte order */
	char *s_proto;    /* Service communication protocol */
};


struct addrinfo {
	int ai_flags;             /* Input flags */
	int ai_family;            /* Socket address family */
	int ai_socktype;          /* Socket type */
	int ai_protocol;          /* Socket protocol */
	socklen_t ai_addrlen;     /* Socket address length */
	struct sockaddr *ai_addr; /* Socket address */
	char *ai_canonname;       /* Canonical name of service location */
	struct addrinfo *ai_next; /* Next structure */
};


extern int h_errno; /* FIXME: not thread-safe */


extern const char *hstrerror(int err);


extern struct servent *getservbyname(const char *name, const char *proto);


extern struct servent *getservbyport(int port, const char *proto);


extern struct hostent *gethostbyname(const char *name);


extern struct hostent *gethostbyaddr(const void *addr, socklen_t len, int type);


extern int getnameinfo(const struct sockaddr *addr, socklen_t addrlen, char *host, socklen_t hostlen, char *serv, socklen_t servlen, int flags);


extern int getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res);


extern void freeaddrinfo(struct addrinfo *res);


extern const char *gai_strerror(int errcode);


extern struct protoent *getprotobyname(const char *name);


extern struct protoent *getprotobynumber(int proto);


#endif
