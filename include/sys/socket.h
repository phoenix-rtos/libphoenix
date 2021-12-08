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

#ifndef _LIBPHOENIX_SYS_SOCKET_H_
#define _LIBPHOENIX_SYS_SOCKET_H_

#include <phoenix/posix/socket.h>


extern int socket(int domain, int type, int protocol);


extern int connect(int socket, const struct sockaddr *address, socklen_t address_len);


extern int bind(int socket, const struct sockaddr *address, socklen_t address_len);


extern int listen(int socket, int backlog);


extern int accept4(int socket, struct sockaddr *address, socklen_t *address_len, int flags);


extern int accept(int socket, struct sockaddr *address, socklen_t *address_len);


extern ssize_t send(int socket, const void *message, size_t length, int flags);


extern ssize_t sendto(int socket, const void *message, size_t length, int flags, const struct sockaddr *dest_addr, socklen_t dest_len);


extern ssize_t sendmsg(int socket, const struct msghdr *msg, int flags) __attribute__((warning("sendmsg() is not fully supported")));


extern ssize_t recv(int socket, void *message, size_t length, int flags);


extern ssize_t recvfrom(int socket, void *message, size_t length, int flags, struct sockaddr *src_addr, socklen_t *src_len);


extern ssize_t recvmsg(int socket, struct msghdr *msg, int flags) __attribute__((warning("recvmsg() is not fully supported")));


extern int getpeername(int socket, struct sockaddr *address, socklen_t *address_len);


extern int getsockname(int socket, struct sockaddr *address, socklen_t *address_len);


extern int getsockopt(int socket, int level, int optname, void *optval, socklen_t *optlen);


extern int setsockopt(int socket, int level, int optname, const void *optval, socklen_t optlen);


extern int shutdown(int socket, int how);


extern int socketpair(int domain, int type, int protocol, int socket_vector[2]);


#endif
