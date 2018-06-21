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
#include <sys/poll.h>
#include <sys/file.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>


#ifdef ERRNO_IN_RETURN
#define set_errno
#else
static ssize_t set_errno(ssize_t ret)
{
	if (ret < 0) {
		errno = -ret;
		return -1;
	} else
		return ret;
}
#endif


int socket(int domain, int type, int protocol)
{
	oid_t oid;
	msg_t msg = { 0 };
	sockport_msg_t *smi = (void *)msg.i.raw;
	unsigned fd;
	int err;

	if ((err = lookup(PATH_SOCKSRV, &oid)) < 0)
		return set_errno(err);

	msg.type = sockmSocket;
	smi->socket.domain = domain;
	smi->socket.type = type;
	smi->socket.protocol = protocol;

	if ((err = msgSend(oid.port, &msg)) < 0)
		return set_errno(err);
	if (msg.o.lookup.err < 0)
		return set_errno(msg.o.lookup.err);

	if ((err = fileAdd(&fd, &msg.o.lookup.res, S_IFSOCK)) < 0)
		return set_errno(err);

	return fd;
}


static ssize_t sockcall(int socket, msg_t *msg)
{
	sockport_resp_t *smo = (void *)msg->o.raw;
	unsigned mode;
	oid_t oid;
	int err;

	if (fileGet(socket, 1, &oid, NULL, &mode) != EOK)
		return -EBADF;
	if (!S_ISSOCK(mode))
		return -ENOTSOCK;

	if ((err = msgSend(oid.port, msg)) < 0)
		return set_errno(err);

	return set_errno(smo->ret);
}


static ssize_t socknamecall(int socket, msg_t *msg, struct sockaddr *address, socklen_t *address_len)
{
	sockport_resp_t *smo = (void *)msg->o.raw;
	ssize_t err;

	if ((err = sockcall(socket, msg)) < 0)
		return err;

	if (smo->sockname.addrlen > *address_len)
		smo->sockname.addrlen = *address_len;

	memcpy(address, smo->sockname.addr, smo->sockname.addrlen);
	*address_len = smo->sockname.addrlen;

	return 0;
}


static ssize_t sockdestcall(int socket, msg_t *msg, const struct sockaddr *address, socklen_t address_len)
{
	sockport_msg_t *smi = (void *)msg->i.raw;

	if (address_len > sizeof(smi->send.addr))
		return set_errno(-EINVAL);

	smi->send.addrlen = address_len;
	memcpy(smi->send.addr, address, address_len);

	return sockcall(socket, msg);
}


int connect(int socket, const struct sockaddr *address, socklen_t address_len)
{
	msg_t msg = { 0 };

	msg.type = sockmSend;

	return sockdestcall(socket, &msg, address, address_len);
}


int bind(int socket, const struct sockaddr *address, socklen_t address_len)
{
	msg_t msg = { 0 };

	msg.type = sockmBind;

	return sockdestcall(socket, &msg, address, address_len);
}


int listen(int socket, int backlog)
{
	msg_t msg = { 0 };
	sockport_msg_t *smi = (void *)msg.i.raw;

	msg.type = sockmListen;
	smi->listen.backlog = backlog;

	return sockcall(socket, &msg);
}


int accept4(int socket, struct sockaddr *address, socklen_t *address_len, int flags)
{
	unsigned fd;
	ssize_t err;
	msg_t msg = { 0 };
	oid_t oid = { 0 };
	sockport_msg_t *smi = (void *)msg.i.raw;

	msg.type = sockmAccept;
	smi->send.flags = flags;

	if ((err = socknamecall(socket, &msg, address, address_len)) < 0)
		return err;
	oid.port = err;

	if ((err = fileAdd(&fd, &oid, 0)) < 0) {
		msg.type = mtClose;
		msgSend(oid.port, &msg);
		return set_errno(err);
	}

	return fd;
}


int accept(int socket, struct sockaddr *address, socklen_t *address_len)
{
	return accept4(socket, address, address_len, 0);
}


ssize_t sendto(int socket, const void *message, size_t length, int flags, const struct sockaddr *dest_addr, socklen_t dest_len)
{
	msg_t msg = { 0 };
	sockport_msg_t *smi = (void *)msg.i.raw;

	msg.type = sockmSend;
	smi->send.flags = flags;
	msg.i.data = (void *)message;
	msg.i.size = length;

	return sockdestcall(socket, &msg, dest_addr, dest_len);
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
		return set_errno(-ENOSYS);	// FIXME: pass ancillary data

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
		return set_errno(-ENOMEM);

	for (sz = 0, i = 0; i < msg->msg_iovlen; ++i) {
		memcpy(buf + sz, msg->msg_iov[i].iov_base, msg->msg_iov[i].iov_len);
		sz += msg->msg_iov[i].iov_len;
	}

	ret = sendto(socket, buf, sz, flags, msg->msg_name, msg->msg_namelen);

	free(buf);

	return ret;
}


ssize_t recvfrom(int socket, void *message, size_t length, int flags, struct sockaddr *src_addr, socklen_t *src_len)
{
	msg_t msg = { 0 };
	sockport_msg_t *smi = (void *)msg.i.raw;

	msg.type = sockmRecv;
	smi->send.flags = flags;
	msg.o.data = message;
	msg.o.size = length;

	return socknamecall(socket, &msg, src_addr, src_len);
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
		return set_errno(-ENOMEM);

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


int getsockname(int socket, struct sockaddr *address, socklen_t *address_len)
{
	msg_t msg = { 0 };

	msg.type = sockmGetSockName;

	return socknamecall(socket, &msg, address, address_len);
}


int getpeername(int socket, struct sockaddr *address, socklen_t *address_len)
{
	msg_t msg = { 0 };

	msg.type = sockmGetPeerName;

	return socknamecall(socket, &msg, address, address_len);
}


int __sock_getfl(int socket)
{
	msg_t msg = { 0 };

	msg.type = sockmGetFl;

	return sockcall(socket, &msg);
}


int __sock_setfl(int socket, int val)
{
	msg_t msg = { 0 };
	sockport_msg_t *smi = (void *)msg.i.raw;

	msg.type = sockmSetFl;
	smi->send.flags = val;

	return sockcall(socket, &msg);
}


int getsockopt(int socket, int level, int optname, void *optval, socklen_t *optlen)
{
	msg_t msg = { 0 };
	sockport_msg_t *smi = (void *)msg.i.raw;
	ssize_t ret;

	msg.type = sockmGetOpt;
	smi->opt.level = level;
	smi->opt.optname = optname;
	msg.o.data = optval;
	msg.o.size = *optlen;

	ret = sockcall(socket, &msg);
	if (ret < 0)
		return ret;

	*optlen = ret;
	return 0;
}


int setsockopt(int socket, int level, int optname, const void *optval, socklen_t optlen)
{
	msg_t msg = { 0 };
	sockport_msg_t *smi = (void *)msg.i.raw;

	msg.type = sockmSetOpt;
	smi->opt.level = level;
	smi->opt.optname = optname;
	msg.i.data = (void *)optval;
	msg.i.size = optlen;

	return sockcall(socket, &msg);
}


int __sock_poll(int socket, int events, time_t timeout)
{
	msg_t msg = { 0 };
	sockport_msg_t *smi = (void *)msg.i.raw;
	int err;

	msg.type = sockmPoll;
	smi->poll.events = events;
	smi->poll.timeout = timeout;

	err = sockcall(socket, &msg);
	return err < 0 ? POLLNVAL : err;
}


int shutdown(int socket, int how)
{
	msg_t msg = { 0 };
	sockport_msg_t *smi = (void *)msg.i.raw;

	msg.type = sockmShutdown;
	smi->send.flags = how;

	return sockcall(socket, &msg);
}
