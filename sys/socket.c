/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/socket.c
 *
 * Copyright 2018, 2021 Phoenix Systems
 * Author: Michal Miroslaw, Maciej Purski, Ziemowit Leszczynski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <sys/sockport.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/debug.h>
#include <sys/minmax.h>
#include <errno.h>
#include <netdb.h>
#include <poll.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <limits.h>

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
WRAP_ERRNO_DEF(int, socketpair, (int domain, int type, int protocol, int *sv), (domain, type, protocol, sv))
WRAP_ERRNO_DEF(int, shutdown, (int socket, int how), (socket, how))
WRAP_ERRNO_DEF(int, setsockopt, (int socket, int level, int optname, const void *optval, socklen_t optlen), (socket, level, optname, optval, optlen))

/* NOTE: msg.iov_len > 1 is not yet supported */
extern ssize_t sys_recvmsg(int socket, struct msghdr *msg, int flags);
/* NOTE: msg.iov_len > 1 is not yet supported */
extern ssize_t sys_sendmsg(int socket, const struct msghdr *msg, int flags);

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


ssize_t recv(int socket, void *message, size_t length, int flags)
{
	return recvfrom(socket, message, length, flags, NULL, 0);
}


static inline ssize_t iov_total_len(const struct iovec *iov, size_t iovcnt)
{
	ssize_t tot_len = 0;
	ssize_t max_len = SSIZE_MAX;

	if (iovcnt > IOV_MAX)
		return -EMSGSIZE;

	while (iovcnt--) {
		if (iov->iov_len > max_len)
			return -EINVAL;
		tot_len += iov->iov_len;
		max_len -= iov->iov_len;
		++iov;
	}

	return tot_len;
}


static inline void copy_from_iov(char *buf, const struct iovec *iov, size_t iovcnt)
{
	while (iovcnt--) {
		memcpy(buf, iov->iov_base, iov->iov_len);
		buf += iov->iov_len;
		++iov;
	}
}


static inline void copy_to_iov(const char *buf, const struct iovec *iov, size_t iovcnt, ssize_t len)
{
	while (iovcnt-- && len > 0) {
		size_t n = min(iov->iov_len, len);
		memcpy(iov->iov_base, buf, n);
		buf += n;
		len -= n;
		++iov;
	}
}


ssize_t sendmsg(int socket, const struct msghdr *msg, int flags)
{
	ssize_t len = iov_total_len(msg->msg_iov, msg->msg_iovlen);

	if (len >= 0) {
		if (msg->msg_iovlen <= 1) {
			len = sys_sendmsg(socket, msg, flags);
		}
		else { /* copy data from scatter-gather buffers to a temporary buffer */
			struct iovec _iov = {
				.iov_len = len
			};
			struct msghdr _msg = {
				.msg_name = msg->msg_name,
				.msg_namelen = msg->msg_namelen,
				.msg_iov = &_iov,
				.msg_iovlen = 1,
				.msg_control = msg->msg_control,
				.msg_controllen = msg->msg_controllen,
				.msg_flags = msg->msg_flags
			};

			if (len <= 64) {
				char buf[64]; /* small buffer optimization */

				_iov.iov_base = buf;
				copy_from_iov(buf, msg->msg_iov, msg->msg_iovlen);
				len = sys_sendmsg(socket, &_msg, flags);
			}
			else {
				void *buf;

				buf = malloc(len);
				if (!buf)
					return SET_ERRNO(-ENOMEM);

				_iov.iov_base = buf;
				copy_from_iov(buf, msg->msg_iov, msg->msg_iovlen);
				len = sys_sendmsg(socket, &_msg, flags);
				free(buf);
			}
		}
	}

	return SET_ERRNO(len);
}


ssize_t recvmsg(int socket, struct msghdr *msg, int flags)
{
	ssize_t len = iov_total_len(msg->msg_iov, msg->msg_iovlen);

	if (len >= 0) {
		if (msg->msg_iovlen <= 1) {
			len = sys_recvmsg(socket, msg, flags);
		}
		else { /* copy data from a temporary buffer to scatter-gather buffers */
			struct iovec _iov = {
				.iov_len = len
			};
			struct msghdr _msg = {
				.msg_name = msg->msg_name,
				.msg_namelen = msg->msg_namelen,
				.msg_iov = &_iov,
				.msg_iovlen = 1,
				.msg_control = msg->msg_control,
				.msg_controllen = msg->msg_controllen,
				.msg_flags = msg->msg_flags
			};

			if (len <= 64) {
				char buf[64]; /* small buffer optimization */

				_iov.iov_base = buf;
				len = sys_recvmsg(socket, &_msg, flags);
				copy_to_iov(buf, msg->msg_iov, msg->msg_iovlen, len);
			}
			else {
				void *buf;

				buf = malloc(len);
				if (!buf)
					return SET_ERRNO(-ENOMEM);

				_iov.iov_base = buf;
				len = sys_recvmsg(socket, &_msg, flags);
				copy_to_iov(buf, msg->msg_iov, msg->msg_iovlen, len);
				free(buf);
			}
		}
	}

	return SET_ERRNO(len);
}


struct servent *getservbyname(const char *name, const char *proto)
{
	debug(__func__);
	debug(" : not implemented\n");
	return NULL;
}


struct servent *getservbyport(int port, const char *proto)
{
	debug(__func__);
	debug(" : not implemented\n");
	return NULL;
}


struct hostent *gethostbyname(const char *name)
{
	debug(__func__);
	debug(" : not implemented\n");
	return NULL;
}


struct hostent *gethostbyaddr(const void *addr, socklen_t len, int type)
{
	debug(__func__);
	debug(" : not implemented\n");
	return NULL;
}


int getnameinfo(const struct sockaddr *addr, socklen_t addrlen,
	char *host, socklen_t hostlen,
	char *serv, socklen_t servlen, int flags)
{
	msg_t msg = { 0 };
	sockport_msg_t *smi = (void *)msg.i.raw;
	sockport_resp_t *smo = (void *)msg.o.raw;
	size_t hlen;
	char *buf;

	if (addrlen > sizeof(smi->send.addr))
		return EAI_FAMILY;

	if (host == NULL)
		hostlen = 0;

	if (serv == NULL)
		servlen = 0;

	if (hostlen == 0 && servlen == 0)
		return EAI_NONAME;

	buf = malloc(hostlen + servlen);
	if (!buf)
		return EAI_MEMORY;

	hlen = hostlen;

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
		if (hostlen > 0) {
			if (hostlen < smo->nameinfo.hostlen) {
				smo->ret = EAI_OVERFLOW;
				memcpy(host, buf, hostlen);
			}
			else
				memcpy(host, buf, smo->nameinfo.hostlen);
		}

		if (servlen > 0) {
			if (servlen < smo->nameinfo.servlen) {
				smo->ret = EAI_OVERFLOW;
				memcpy(serv, buf + hostlen, servlen);
			}
			else
				memcpy(serv, buf + hostlen, smo->nameinfo.servlen);
		}
	}

	if (smo->ret == EAI_SYSTEM)
		(void)SET_ERRNO(-smo->sys.err);

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
			(void)SET_ERRNO(-smo->sys.err);
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


int getifaddrs(struct ifaddrs **ifap)
{
	struct ifaddrs *ifa;
	msg_t msg = { 0 };
	size_t bufsz, namesz, addrsz;
	char *p;
	sockport_resp_t *smo = (sockport_resp_t *)msg.o.raw;
	void *tmp;

	msg.type = sockmGetIfAddrs;
	msg.o.size = 256;

	for (;;) {
		tmp = realloc(msg.o.data, msg.o.size);
		if (!tmp) {
			free(msg.o.data);
			return EAI_MEMORY;
		}
		msg.o.data = tmp;

		if (socksrvcall(&msg) < 0) {
			free(msg.o.data);
			return EAI_SYSTEM;
		}

		if (smo->ret != EAI_OVERFLOW)
			break;

		if (msg.o.size < smo->sys.buflen)
			msg.o.size = smo->sys.buflen;
		else
			msg.o.size *= 2;
	}

	bufsz = smo->sys.buflen;
	if (smo->ret || bufsz > msg.o.size) {
		free(msg.o.data);
		if (smo->ret == EAI_SYSTEM)
			(void)SET_ERRNO(-smo->sys.err);
		return smo->ret;
	}

	*ifap = msg.o.data;

	for (ifa = msg.o.data; ifa; ifa = ifa->ifa_next) {
		if (bufsz < (uintptr_t)ifa - (uintptr_t)msg.o.data + sizeof(*ifa))
			goto out_overflow;

		if (ifa->ifa_addr) {
			if (bufsz < (uintptr_t)ifa->ifa_addr + sizeof(sa_family_t))
				goto out_overflow;

			/* First byte of sa_family field contains addr len */
			addrsz = *(uint8_t *)((uintptr_t)msg.o.data + (uintptr_t)ifa->ifa_addr);
			if (bufsz < (uintptr_t)ifa->ifa_addr + addrsz)
				goto out_overflow;

			ifa->ifa_addr = msg.o.data + (uintptr_t)ifa->ifa_addr;
			/* This shift converts lwip sa_family field format to libphoenix */
			ifa->ifa_addr->sa_family >>= 8;
		}

		if (ifa->ifa_dstaddr) {
			if (bufsz < (uintptr_t)ifa->ifa_dstaddr + sizeof(sa_family_t))
				goto out_overflow;

			addrsz = *(uint8_t *)(msg.o.data + (uintptr_t)ifa->ifa_dstaddr);
			if (bufsz < (uintptr_t)ifa->ifa_dstaddr + addrsz)
				goto out_overflow;

			ifa->ifa_dstaddr = msg.o.data + (uintptr_t)ifa->ifa_dstaddr;
			ifa->ifa_dstaddr->sa_family >>= 8;
		}

		if (ifa->ifa_netmask) {
			if (bufsz < (uintptr_t)ifa->ifa_netmask + sizeof(sa_family_t))
				goto out_overflow;

			addrsz = *(uint8_t *)(msg.o.data + (uintptr_t)ifa->ifa_netmask);
			if (bufsz < (uintptr_t)ifa->ifa_netmask + addrsz)
				goto out_overflow;

			ifa->ifa_netmask = msg.o.data + (uintptr_t)ifa->ifa_netmask;
			ifa->ifa_netmask->sa_family >>= 8;
		}

		if (ifa->ifa_name) {
			if (bufsz <= (uintptr_t)ifa->ifa_name)
				goto out_overflow;

			p = msg.o.data + (uintptr_t)ifa->ifa_name;
			namesz = strnlen(p, bufsz - (uintptr_t)ifa->ifa_name);
			if (bufsz < (uintptr_t)ifa->ifa_name + namesz)
				goto out_overflow;

			ifa->ifa_name = p;
		}

		if (ifa->ifa_next) {
			if (bufsz < (uintptr_t)ifa->ifa_next)
				goto out_overflow;
			ifa->ifa_next = msg.o.data + (uintptr_t)ifa->ifa_next;
		}
	}

	return 0;
out_overflow:
	free(msg.o.data);
	(void)SET_ERRNO(EPROTO);
	return EAI_SYSTEM;
}


void freeifaddrs(struct ifaddrs *ifa)
{
	free(ifa);
}

const char *hstrerror(int err)
{
	switch (err) {
		case RESOLVED_OK: return "SUCCESS";
		case HOST_NOT_FOUND: return "Host not found";
		case TRY_AGAIN: return "Try again later";
		case NO_RECOVERY: return "A nonrecoverable name server error occurred";
		case NO_DATA: return "The requested name is valid but does not have an IP address.";
	}

	return "Unknown error";
}
