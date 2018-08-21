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


WRAP_ERRNO_DEF(int, accept, (int socket, struct sockaddr *address, socklen_t *address_len), (socket, address, address_len))
WRAP_ERRNO_DEF(int, accept4, (int socket, struct sockaddr *address, socklen_t *address_len, int flags), (socket, address, address_len, flags))
WRAP_ERRNO_DEF(int, bind, (int socket, const struct sockaddr *address, socklen_t address_len), (socket, address, address_len))
WRAP_ERRNO_DEF(int, connect, (int socket, const struct sockaddr *address, socklen_t address_len), (socket, address, address_len))
WRAP_ERRNO_DEF(int, getpeername, (int socket, struct sockaddr *address, socklen_t *address_len), (socket, address, address_len))
WRAP_ERRNO_DEF(int, getsockname, (int socket, struct sockaddr *address, socklen_t *address_len), (socket, address, address_len))
WRAP_ERRNO_DEF(int, getsockopt, (int socket, int level, int optname, void *optval, socklen_t *optlen), (socket, level, optname, optval, optlen))
WRAP_ERRNO_DEF(int, listen, (int socket, int backlog), (socket, backlog))
WRAP_ERRNO_DEF(ssize_t, recvfrom, (int socket, void *message, size_t length, int flags, struct sockaddr *src_addr, socklen_t *src_len), (socket, message, length, flags, src_addr, src_len))
WRAP_ERRNO_DEF(ssize_t, sendto, (int socket, const void *message, size_t length, int flags, const struct sockaddr *dest_addr, socklen_t dest_len), (socket, message, length, flags, dest_addr, dest_len))
WRAP_ERRNO_DEF(int, socket, (int domain, int type, int protocol), (domain, type, protocol))
WRAP_ERRNO_DEF(int, shutdown, (int socket, int how), (socket, how))
WRAP_ERRNO_DEF(int, setsockopt, (int socket, int level, int optname, const void *optval, socklen_t optlen), (socket, level, optname, optval, optlen))


int h_errno;

static int socksrvcall(msg_t *msg)
{
	oid_t oid;
	int err;

	if ((err = lookup(PATH_SOCKSRV, NULL, &oid)) < 0)
		return SET_ERRNO(err);
	if ((err = msgSend(oid.port, msg)) < 0)
		return SET_ERRNO(err);
	return 0;
}


ssize_t send(int socket, const void *message, size_t length, int flags)
{
	return sendto(socket, message, length, flags, NULL, 0);
}


static size_t iov_total_len(const struct iovec *iov, size_t n)
{
	size_t sz = 0;

	while (n--)
		sz += (iov++)->iov_len;

	return sz;
}


// FIXME: no scatter-gather support in kernel
ssize_t sendmsg(int socket, const struct msghdr *msg, int flags)
{
	ssize_t ret;
	size_t sz = 0, i;
	void *buf = NULL;

	if (msg->msg_controllen)
		return SET_ERRNO(-ENOSYS);	// FIXME: pass ancillary data

	sz = iov_total_len(msg->msg_iov, msg->msg_iovlen);
	if (msg->msg_iovlen <= 1 || !sz) {
		if (msg->msg_iovlen) {
			buf = msg->msg_iov->iov_base;
			sz = msg->msg_iov->iov_len;
		}
		return sendto(socket, buf, sz, flags, msg->msg_name, msg->msg_namelen);
	}

	buf = malloc(sz);
	if (!buf)
		return SET_ERRNO(-ENOMEM);

	for (sz = 0, i = 0; i < msg->msg_iovlen; ++i) {
		memcpy(buf + sz, msg->msg_iov[i].iov_base, msg->msg_iov[i].iov_len);
		sz += msg->msg_iov[i].iov_len;
	}

	ret = sendto(socket, buf, sz, flags, msg->msg_name, msg->msg_namelen);

	free(buf);

	return ret;
}


ssize_t recv(int socket, void *message, size_t length, int flags)
{
	return recvfrom(socket, message, length, flags, NULL, 0);
}


// FIXME: no scatter-gather support in kernel
ssize_t recvmsg(int socket, struct msghdr *msg, int flags)
{
	ssize_t ret;
	size_t sz = 0, i;
	void *buf = NULL;

	msg->msg_controllen = 0;	// FIXME: pass ancillary data
	msg->msg_flags = 0;

	sz = iov_total_len(msg->msg_iov, msg->msg_iovlen);
	if (msg->msg_iovlen <= 1 || !sz) {
		if (msg->msg_iovlen) {
			buf = msg->msg_iov->iov_base;
			sz = msg->msg_iov->iov_len;
		}
		return recvfrom(socket, buf, sz, flags, msg->msg_name, &msg->msg_namelen);
	}

	buf = malloc(sz);
	if (!buf)
		return SET_ERRNO(-ENOMEM);

	ret = recvfrom(socket, buf, sz, flags, msg->msg_name, &msg->msg_namelen);

	if (ret > 0) {
		for (sz = 0, i = 0; i < msg->msg_iovlen && sz < ret; ++i) {
			size_t left = ret - sz;
			if (left > msg->msg_iov[i].iov_len)
				left = msg->msg_iov[i].iov_len;
			memcpy(msg->msg_iov[i].iov_base, buf + sz, left);
			sz += left;
		}
	}

	free(buf);

	return ret;
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


int getnameinfo(const struct sockaddr *addr, socklen_t addrlen,
                char *host, socklen_t hostlen,
                char *serv, socklen_t servlen, int flags)
{
	msg_t msg = { 0 };
	sockport_msg_t *smi = (void *)msg.i.raw;
	sockport_resp_t *smo = (void *)msg.o.raw;
	size_t hlen = hostlen;
	char *buf;

	if (addrlen > sizeof(smi->send.addr))
		return EAI_FAMILY;

	buf = malloc(hostlen + servlen);
	if (!buf)
		return EAI_MEMORY;

	msg.type = sockmGetNameInfo;
	smi->send.flags = flags;
	smi->send.addrlen = addrlen;
	memcpy(smi->send.addr, addr, addrlen);
	msg.i.data = &hlen;
	msg.i.size = sizeof(hlen);
	msg.o.data = buf;
	msg.o.size = hostlen + servlen;

	if (socksrvcall(&msg) < 0) {
		free(buf);
		return EAI_SYSTEM;
	}

	if (!smo->ret) {
		if (hostlen < smo->nameinfo.hostlen)
			smo->ret = EAI_OVERFLOW;
		if (servlen < smo->nameinfo.servlen)
			smo->ret = EAI_OVERFLOW;
	}

	if (!smo->ret) {
		memcpy(host, buf, smo->nameinfo.hostlen);
		memcpy(serv, buf + smo->nameinfo.hostlen, smo->nameinfo.servlen);
	}

	if (smo->ret == EAI_SYSTEM)
		(void)SET_ERRNO(-smo->sys.errno);

	free(buf);
	return smo->ret;
}


int getaddrinfo(const char *node, const char *service,
                const struct addrinfo *hints,
                struct addrinfo **res)
{
	struct addrinfo *ai;
	msg_t msg = { 0 };
	sockport_msg_t *smi = (void *)msg.i.raw;
	sockport_resp_t *smo = (void *)msg.o.raw;
	size_t nodesz, servsz, bufsz;
	char *p;

	if (hints && (hints->ai_addrlen || hints->ai_canonname || hints->ai_next))
		return EAI_BADFLAGS;

	nodesz = node ? strlen(node) + 1 : 0;
	servsz = service ? strlen(service) + 1 : 0;

	msg.type = sockmGetAddrInfo;
	smi->socket.domain = hints ? hints->ai_family : AF_UNSPEC;
	smi->socket.type = hints ? hints->ai_socktype : 0;
	smi->socket.protocol = hints ? hints->ai_protocol : 0;
	smi->socket.flags = hints ? hints->ai_flags : (AI_V4MAPPED | AI_ADDRCONFIG);
	smi->socket.ai_node_sz = nodesz;

	if (nodesz + servsz) {
		msg.i.size = nodesz + servsz;
		msg.i.data = malloc(msg.i.size);
		if (!msg.i.data)
			return EAI_MEMORY;
		memcpy(msg.i.data, node, nodesz);
		memcpy(msg.i.data + nodesz, service, servsz);
	}

	msg.o.size = 128;
	for (;;) {
		void *buf = realloc(msg.o.data, msg.o.size);
		if (!buf) {
			realloc(msg.i.data, 0);
			realloc(msg.o.data, 0);
			return EAI_MEMORY;
		}
		msg.o.data = buf;

		if (socksrvcall(&msg) < 0) {
			realloc(msg.i.data, 0);
			realloc(msg.o.data, 0);
			return EAI_SYSTEM;
		}

		if (smo->ret != EAI_OVERFLOW)
			break;

		if (msg.o.size < smo->sys.buflen)
			msg.o.size = smo->sys.buflen;
		else
			msg.o.size *= 2;
	}

	realloc(msg.i.data, 0);

	bufsz = smo->sys.buflen;
	if (smo->ret || bufsz > msg.o.size) {
		free(msg.o.data);
		if (smo->ret == EAI_SYSTEM)
			(void)SET_ERRNO(-smo->sys.errno);
		return smo->ret;
	}

	*res = msg.o.data = realloc(msg.o.data, bufsz);

	for (ai = msg.o.data; ai; ai = ai->ai_next) {
		if (bufsz < (void *)ai - msg.o.data + sizeof(*ai))
			goto out_overflow;

		if (ai->ai_addrlen) {
			if (bufsz < (uintptr_t)ai->ai_addr + ai->ai_addrlen)
				goto out_overflow;
			ai->ai_addr = msg.o.data + (uintptr_t)ai->ai_addr;
		}

		if (ai->ai_canonname) {
			if (bufsz <= (uintptr_t)ai->ai_canonname)
				goto out_overflow;

			p = msg.o.data + (uintptr_t)ai->ai_canonname;
			nodesz = strnlen(p, bufsz - (uintptr_t)ai->ai_canonname) + 1;

			if (bufsz < (uintptr_t)ai->ai_canonname + nodesz)
				goto out_overflow;

			ai->ai_canonname = p;
		}

		if (ai->ai_next) {
			if (bufsz < (uintptr_t)ai->ai_next)
				goto out_overflow;
			ai->ai_next = msg.o.data + (uintptr_t)ai->ai_next;
		}
	}

	return 0;

out_overflow:
	free(msg.o.data);
	(void)SET_ERRNO(EPROTO);
	return EAI_SYSTEM;
}


void freeaddrinfo(struct addrinfo *res)
{
	free(res);
}
