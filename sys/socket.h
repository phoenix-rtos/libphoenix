/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/socket.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _SYS_SOCKET_H_
#define _SYS_SOCKET_H_

typedef int socklen_t;
typedef unsigned int sa_family_t;

#define AF_UNIX 0
#define AF_UNSPEC 1
#define AF_INET 2
#define AF_INET6 3

#define SOCK_STREAM 0
//#define SOCK_DGRAM 1
#define SOCK_RDM 2
#define SOCK_DGRAM 3
#define SOCK_SEQPACKET 4
#define SOCK_RAW 5

#define SOL_SOCKET 0
#define SO_ACCEPTCONN 1
#define SO_BROADCAST 2
#define SO_DEBUG 3
#define SO_DONTROUTE 4
#define SO_ERROR 5
#define SO_KEEPALIVE 6
#define SO_LINGER 7
#define SO_OOBINLINE 8
#define SO_RCVBUF 9
#define SO_RCVLOWAT 10
#define SO_RCVTIMEO 11
#define SO_REUSEADDR 12
#define SO_SNDBUF 13
#define SO_SNDLOWAT 14
#define SO_SNDTIMEO 15
#define SO_TYPE 16


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


int getpeername(int socket, struct sockaddr *address, socklen_t *address_len);


int getsockname(int socket, struct sockaddr *address, socklen_t *address_len);


#endif
