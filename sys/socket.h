/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/socket.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski, Michał Mirosław
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _SYS_SOCKET_H_
#define _SYS_SOCKET_H_

#include <stdint.h>

typedef int socklen_t;
typedef unsigned int sa_family_t;

#define AF_UNSPEC 0
#define AF_UNIX 1
#define AF_INET 2
#define AF_INET6 10

#define SOCK_STREAM 1
#define SOCK_DGRAM 2
#define SOCK_RAW 3
#define SOCK_SEQPACKET 4
#define SOCK_RDM 5

#define SOL_SOCKET 0xFFF
#define SO_ACCEPTCONN 0x0002
#define SO_BROADCAST 0x0020
#define SO_DEBUG 0x0001
#define SO_DONTROUTE 0x0010
#define SO_ERROR 0x1007
#define SO_KEEPALIVE 0x0008
#define SO_LINGER 0x0080
#define SO_OOBINLINE 0x0100
#define SO_RCVBUF 0x1002
#define SO_RCVLOWAT 0x1004
#define SO_RCVTIMEO 0x1006
#define SO_REUSEADDR 0x0004
#define SO_SNDBUF 0x1001
#define SO_SNDLOWAT 0x1003
#define SO_SNDTIMEO 0x1005
#define SO_TYPE 0x1008


struct sockaddr {
	sa_family_t sa_family;
	char        sa_data[];

};


struct msghdr {
	void         *msg_name;
	socklen_t     msg_namelen;
	struct iovec *msg_iov;
	int           msg_iovlen;
	void         *msg_control;
	socklen_t     msg_controllen;
	int           msg_flags;
};


struct cmsghdr {
	socklen_t cmsg_len;
	int       cmsg_level;
	int       cmsg_type;
};


int socket(int domain, int type, int protocol);
int connect(int socket, const struct sockaddr *address, socklen_t address_len);
int bind(int socket, const struct sockaddr *address, socklen_t address_len);
int listen(int socket, int backlog);
int accept(int socket, struct sockaddr *address, socklen_t *address_len);
ssize_t sendto(int socket, const void *message, size_t length, int flags, const struct sockaddr *dest_addr, socklen_t dest_len);
ssize_t send(int socket, const void *message, size_t length, int flags);
ssize_t recvfrom(int socket, void *message, size_t length, int flags, struct sockaddr *src_addr, socklen_t *src_len);
ssize_t recv(int socket, void *message, size_t length, int flags);
int getpeername(int socket, struct sockaddr *address, socklen_t *address_len);
int getsockname(int socket, struct sockaddr *address, socklen_t *address_len);


#endif
