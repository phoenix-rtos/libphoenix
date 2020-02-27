/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/sicjet.c
 *
 * Copyright 2018 Phoenix Systems
 * Author: Michał Mirosław
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <sys/sockport.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <errno.h>
#include <netdb.h>
#include <poll.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>

WRAP_ERRNO_DEF(int, accept4, (int socket, struct sockaddr *address, socklen_t *address_len, int flags), (socket, address, address_len, flags))
WRAP_ERRNO_DEF(int, bind, (int socket, const struct sockaddr *address, socklen_t address_len), (socket, address, address_len))
WRAP_ERRNO_DEF(int, connect, (int socket, const struct sockaddr *address, socklen_t address_len), (socket, address, address_len))
WRAP_ERRNO_DEF(int, getpeername, (int socket, struct sockaddr *address, socklen_t *address_len), (socket, address, address_len))
WRAP_ERRNO_DEF(int, getsockname, (int socket, struct sockaddr *address, socklen_t *address_len), (socket, address, address_len))
WRAP_ERRNO_DEF(int, getsockopt, (int socket, int level, int optname, void *optval, socklen_t *optlen), (socket, level, optname, optval, optlen))
WRAP_ERRNO_DEF(int, listen, (int socket, int backlog), (socket, backlog))
WRAP_ERRNO_DEF(int, socket, (int domain, int type, int protocol), (domain, type, protocol))
WRAP_ERRNO_DEF(int, shutdown, (int socket, int how), (socket, how))
WRAP_ERRNO_DEF(int, setsockopt, (int socket, int level, int optname, const void *optval, socklen_t optlen), (socket, level, optname, optval, optlen))


int h_errno;


int accept(int socket, struct sockaddr *address, socklen_t *address_len)
{
	return accept4(socket, address, address_len, 0);
}


ssize_t sendmsg(int socket, const struct msghdr *msg, int flags)
{
	return SET_ERRNO(sys_sendmsg(socket, msg, flags));
}


ssize_t sendto(int socket, const void *message, size_t length, int flags, const struct sockaddr *dest_addr, socklen_t dest_len)
{
	struct msghdr msg;
	struct iovec iov;

	iov.iov_base = message;
	iov.iov_len = length;

	msg.msg_name = dest_addr;
	msg.msg_namelen = dest_len;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_control = NULL;
	msg.msg_flags = flags;

	return sendmsg(socket, &msg, flags);
}


ssize_t send(int socket, const void *message, size_t length, int flags)
{
	return sendto(socket, message, length, flags, NULL, 0);
}



ssize_t recvmsg(int socket, struct msghdr *msg, int flags)
{
	return SET_ERRNO(sys_recvmsg(socket, msg, flags));
}


ssize_t recvfrom(int socket, void *buffer, size_t length, int flags, struct sockaddr *address, socklen_t *address_len)
{
	struct msghdr msg;
	struct iovec iov;
	ssize_t retval;

	iov.iov_base = buffer;
	iov.iov_len = length;

	msg.msg_name = address;
	msg.msg_namelen = address == NULL ? 0 : *address_len;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_control = NULL;
	msg.msg_flags = flags;

	if ((retval = recvmsg(socket, &msg, flags)) != -1 && address_len != NULL)
		*address_len = msg.msg_namelen;

	return retval;
}


ssize_t recv(int socket, void *message, size_t length, int flags)
{
	return recvfrom(socket, message, length, flags, NULL, 0);
}


struct servent *getservbyname(const char *name, const char *proto)
{
	return NULL;
}


struct servent *getservbyport(int port, const char *proto)
{
	return NULL;
}


struct hostent *gethostbyname(const char *name)
{
	return NULL;
}


struct hostent *gethostbyaddr(const void *addr, socklen_t len, int type)
{
	return NULL;
}


int getnameinfo(const struct sockaddr *sa, socklen_t addrlen, char *host, socklen_t hostsz, char *serv, socklen_t servsz, int flags)
{
	// TODO: implement real netdb (for now always return the IP representation)
	if (sa == NULL)
		return EAI_FAIL;

	if (sa->sa_family == AF_INET) {
		struct sockaddr_in *sa_in = (struct sockaddr_in *)sa;

		if (host != NULL) {
			snprintf(host, hostsz, "%u.%u.%u.%u", (unsigned char)sa->sa_data[2], (unsigned char)sa->sa_data[3],
				(unsigned char)sa->sa_data[4], (unsigned char)sa->sa_data[5]);
		}

		if (serv != NULL) {
			snprintf(serv, servsz, "%u", ntohs(sa_in->sin_port));
		}

		return 0;
	}

	return EAI_FAMILY;
}


int getaddrinfo(const char *node, const char *service,
                const struct addrinfo *hints,
                struct addrinfo **res)
{
	struct addrinfo *ai;
	struct sockaddr_in *sa;

	socklen_t sa_len;
	int port;
	struct in_addr in_addr;

	if (hints && (hints->ai_addrlen || hints->ai_canonname || hints->ai_next))
		return EAI_BADFLAGS;

	if ((ai = malloc(sizeof(*ai))) == NULL)
		return EAI_MEMORY;

	if (node == NULL) {
		inet_aton("0.0.0.0", &in_addr);
	}
	else {
		if (!inet_aton(node, &in_addr))
			return EAI_NONAME;
	}

	if ((port = atoi(service)) <= 0 || port >= 65536)
		return EAI_NONAME;

	if ((sa = malloc(sizeof(struct sockaddr_in))) == NULL)
		return EAI_MEMORY;

	sa->sin_family = AF_INET;
	memcpy(&sa->sin_addr, &in_addr, sizeof(in_addr));
	sa->sin_port = htons(port);

	ai->ai_flags = 0;
	ai->ai_family = AF_INET;
	ai->ai_socktype = SOCK_STREAM;
	ai->ai_protocol = 0;
	ai->ai_addrlen = sizeof(struct sockaddr_in);
	ai->ai_addr = sa;
	ai->ai_canonname = 0;
	ai->ai_next = NULL;

	*res = ai;
	return 0;
}


void freeaddrinfo(struct addrinfo *res)
{
	free(res->ai_addr);
	free(res);
}


int socketpair(int domain, int type, int protocol, int socket_vector[2])
{
	int sfd;
	int flags;
	int err;
	struct sockaddr_in serv = { 0 };
	socklen_t socklen;
	/* we don't have nonblocking connections on unix sockets
	 * so we immitate it with inet sockets connected through loopback device */
	sfd = socket(AF_INET, type, protocol);
	if (sfd < 0)
		return sfd;

	flags = fcntl(sfd, F_GETFL);
	err = fcntl(sfd, F_SETFL, flags | O_NONBLOCK);

	if (err < 0) {
		close(sfd);
		return err;
	}

	serv.sin_family = AF_INET;
	serv.sin_port = 0;
	serv.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	socklen = sizeof(serv);

	if ((err = bind(sfd, (struct sockaddr *)&serv, socklen)) || (err = listen(sfd, 2))) {
		close(sfd);
		return err;
	}

	socket_vector[0] = socket(domain, type, protocol);
	if (socket_vector[0] < 0) {
		close(sfd);
		return socket_vector[0];
	}

	flags = fcntl(socket_vector[0], F_GETFL);
	err = fcntl(socket_vector[0], F_SETFL, flags | O_NONBLOCK);

	if (err < 0) {
		close(sfd);
		close(socket_vector[0]);
		return err;
	}

	if ((err = connect(socket_vector[0], (struct sockaddr *)&serv, socklen)) != EINPROGRESS) {
		close(sfd);
		close(socket_vector[0]);
		return err;
	}

	/* TODO: check for accidental connection from another process */
	socket_vector[1] = accept(sfd, (struct sockaddr *)&serv, &socklen);
	if (socket_vector[0] < 0) {
		close(sfd);
		close(socket_vector[0]);
		return socket_vector[1];
	}

	close(sfd);
	flags = fcntl(socket_vector[0], F_GETFL);
	err = fcntl(socket_vector[0], F_SETFL, flags | O_NONBLOCK);

	if (err < 0) {
		close(socket_vector[0]);
		close(socket_vector[1]);
		return err;
	}

	return 0;
}
