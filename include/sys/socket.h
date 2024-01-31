/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/socket.h
 *
 * Copyright 2018, 2023 Phoenix Systems
 * Author: Jan Sikorski, Michal Miroslaw, Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _SYS_SOCKET_H_
#define _SYS_SOCKET_H_

#include <stdint.h>
#include <sys/socktypes.h>
#include <sys/sockdefs.h>
#include <sys/uio.h>

#define AF_UNSPEC 0
#define AF_UNIX   1
#define AF_INET   2
#define AF_INET6  10
#define AF_KEY    15
#define AF_PACKET 17

#define PF_UNSPEC AF_UNSPEC
#define PF_UNIX   AF_UNIX
#define PF_INET   AF_INET
#define PF_INET6  AF_INET6
#define PF_KEY    AF_KEY
#define PF_PACKET AF_PACKET

#define SOCK_STREAM    1
#define SOCK_DGRAM     2
#define SOCK_RAW       3
#define SOCK_SEQPACKET 4
#define SOCK_RDM       5

#define SOL_SOCKET      0xFFF
#define SO_ACCEPTCONN   0x0002
#define SO_BROADCAST    0x0020
#define SO_DEBUG        0x0001
#define SO_DONTROUTE    0x0010
#define SO_ERROR        0x1007
#define SO_KEEPALIVE    0x0008
#define SO_LINGER       0x0080
#define SO_OOBINLINE    0x0100
#define SO_RCVBUF       0x1002
#define SO_RCVLOWAT     0x1004
#define SO_RCVTIMEO     0x1006
#define SO_REUSEADDR    0x0004
#define SO_SNDBUF       0x1001
#define SO_SNDLOWAT     0x1003
#define SO_SNDTIMEO     0x1005
#define SO_TYPE         0x1008
#define SO_BINDTODEVICE 0x100b

#define MSG_OOB      0x01
#define MSG_PEEK     0x02
#define MSG_DONTWAIT 0x08
#define MSG_NOSIGNAL 0x20

#define SHUT_RD   0
#define SHUT_WR   1
#define SHUT_RDWR 2

#define SCM_RIGHTS 1


#ifdef __cplusplus
extern "C" {
#endif


/* Match lwIP size: u8 len + u8 family. */
typedef uint16_t sa_family_t;


struct sockaddr {
	sa_family_t sa_family;
	char sa_data[14]; /* actual size may be bigger */
};


struct sockaddr_storage {
	sa_family_t ss_family;
	char ss_data[128 - sizeof(sa_family_t)]; /* e.g. struct sockaddr_un needs some space */
} __attribute__((aligned(8)));               /* Align to allow safe sockaddr_* casting */


struct linger {
	int l_onoff;
	int l_linger;
};

/* has to be included after sockaddr is defined */
#include <sys/sockios.h>


struct msghdr {
	void *msg_name;
	socklen_t msg_namelen;
	struct iovec *msg_iov;
	int msg_iovlen;
	void *msg_control;
	socklen_t msg_controllen;
	int msg_flags;
};


struct cmsghdr {
	socklen_t cmsg_len;
	int cmsg_level;
	int cmsg_type;
};


#define CMSG_ALIGN(n) (((n) + sizeof(socklen_t) - 1) & ~(sizeof(socklen_t) - 1))
#define CMSG_SPACE(n) (sizeof(struct cmsghdr) + CMSG_ALIGN(n))
#define CMSG_LEN(n)   (sizeof(struct cmsghdr) + (n))
#define CMSG_DATA(c)  ((unsigned char *)((struct cmsghdr *)(c) + 1))

#define CMSG_FIRSTHDR(m) \
	({ \
		struct msghdr *_m = (struct msghdr *)(m); \
		_m->msg_controllen < sizeof(struct cmsghdr) ? NULL : (struct cmsghdr *)_m->msg_control; \
	})

#define CMSG_NXTHDR(m, c) \
	({ \
		struct msghdr *_m = (struct msghdr *)(m); \
		struct cmsghdr *_c = (struct cmsghdr *)(c); \
		char *n = (char *)_c + CMSG_SPACE(_c->cmsg_len); \
		char *e = (char *)_m->msg_control + _m->msg_controllen; \
		(n > e ? NULL : (struct cmsghdr *)n); \
	})


int socket(int domain, int type, int protocol);
int connect(int socket, const struct sockaddr *address, socklen_t address_len);
int bind(int socket, const struct sockaddr *address, socklen_t address_len);
int listen(int socket, int backlog);
int accept4(int socket, struct sockaddr *address, socklen_t *address_len, int flags);
int accept(int socket, struct sockaddr *address, socklen_t *address_len);
ssize_t send(int socket, const void *message, size_t length, int flags);
ssize_t sendto(int socket, const void *message, size_t length, int flags, const struct sockaddr *dest_addr, socklen_t dest_len);
ssize_t sendmsg(int socket, const struct msghdr *msg, int flags) __attribute__((warning("sendmsg() is not fully supported")));
ssize_t recv(int socket, void *message, size_t length, int flags);
ssize_t recvfrom(int socket, void *message, size_t length, int flags, struct sockaddr *src_addr, socklen_t *src_len);
ssize_t recvmsg(int socket, struct msghdr *msg, int flags) __attribute__((warning("recvmsg() is not fully supported")));
int getpeername(int socket, struct sockaddr *address, socklen_t *address_len);
int getsockname(int socket, struct sockaddr *address, socklen_t *address_len);
int getsockopt(int socket, int level, int optname, void *optval, socklen_t *optlen);
int setsockopt(int socket, int level, int optname, const void *optval, socklen_t optlen);
int shutdown(int socket, int how);
int socketpair(int domain, int type, int protocol, int socket_vector[2]);


#ifdef __cplusplus
}
#endif


#endif
