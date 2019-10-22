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
#include <sys/socktypes.h>
#include <sys/sockdefs.h>
#include <sys/uio.h>
#include <phoenix/socket.h>

typedef uint16_t sa_family_t;	// match lwIP size: u8 len + u8 family


struct sockaddr_storage {
	sa_family_t ss_family;
	char        ss_data[128-sizeof(sa_family_t)];
};


struct linger {
	int l_onoff;
	int l_linger;
};

// has to be included after sockaddr is defined
#include <sys/sockios.h>


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


#define CMSG_ALIGN(x) (((x) + _Alignof(struct cmsghdr) - 1) & ~_Alignof(struct cmsghdr))
#define CMSG_SPACE(x) (CMSG_ALIGN(x) + sizeof(struct cmsghdr))
#define CMSG_LEN(x) (x)
#define CMSG_DATA(x) ((unsigned char *)((x) + 1))

static inline struct cmsghdr *CMSG_FIRSTHDR(struct msghdr *m)
{
	return m->msg_controllen ? m->msg_control : NULL;
}

static inline struct cmsghdr *CMSG_NXTHDR(struct msghdr *m, struct cmsghdr *c)
{
	c = (void *)c + CMSG_SPACE(c->cmsg_len);
	return (void *)c < m->msg_control + m->msg_controllen ? c : NULL;
}


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
int __sock_getfl(int socket);
int __sock_setfl(int socket, int val);
int getsockopt(int socket, int level, int optname, void *optval, socklen_t *optlen);
int setsockopt(int socket, int level, int optname, const void *optval, socklen_t optlen);
int shutdown(int socket, int how);
int socketpair(int domain, int type, int protocol, int socket_vector[2]);

#endif
