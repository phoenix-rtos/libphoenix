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
#include <sys/file.h>
#include <errno.h>
#include <string.h>


// FIXME: return -1 and set errno, instead of returning -errno


int socket(int domain, int type, int protocol)
{
	oid_t oid;
	msg_t msg = { 0 };
	sockport_msg_t *smi = (void *)msg.i.raw;
	unsigned fd;
	int err;

	if ((err = lookup(PATH_SOCKSRV, &oid)) != EOK)
		return err;

	msg.type = sockmSocket;
	smi->socket.domain = domain;
	smi->socket.type = type;
	smi->socket.protocol = protocol;

	if ((err = msgSend(oid.port, &msg)) != EOK)
		return err;
	if (msg.o.lookup.err < 0)
		return msg.o.lookup.err;

	if ((err = fileAdd(&fd, &msg.o.lookup.res, 0)) < 0)
		return err;

	return fd;
}


static ssize_t sockcall(int socket, msg_t *msg)
{
	sockport_resp_t *smo = (void *)msg->o.raw;
	oid_t oid;
	int err;

	if (fileGet(socket, 1, &oid, NULL, NULL) != EOK)
		return -EBADF;
	// FIXME: check and report ENOTSOCK?

	if ((err = msgSend(oid.port, msg)) != EOK)
		return err;

	return smo->ret;
}


static ssize_t socknamecall(int socket, msg_t *msg, struct sockaddr *address, socklen_t *address_len)
{
	sockport_resp_t *smo = (void *)msg->o.raw;
	ssize_t err;

	if ((err = sockcall(socket, msg)) != EOK)
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
		return -EINVAL;

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


int accept(int socket, struct sockaddr *address, socklen_t *address_len)
{
	unsigned fd;
	ssize_t err;
	msg_t msg = { 0 };
	oid_t oid = { 0 };

	msg.type = sockmAccept;

	err = socknamecall(socket, &msg, address, address_len);
	if (err < 0)
		return err;
	oid.port = err;

	if ((err = fileAdd(&fd, &oid, 0)) < 0) {
		msg.type = mtClose;
		msgSend(oid.port, &msg);
		return err;
	}

	return fd;
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
