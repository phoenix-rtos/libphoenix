/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/socket.h
 *
 * Copyright 2018, 2023, 2024 Phoenix Systems
 * Author: Jan Sikorski, Michal Miroslaw, Aleksander Kaminski, Lukasz Leczkowski
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


#ifdef __cplusplus
extern "C" {
#endif


#include <phoenix/posix-socket.h>


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
