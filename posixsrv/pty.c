/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * POSIX server - pseudoterminals
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <time.h>
#include <sys/resource.h>
#include <sys/msg.h>
#include <sys/file.h>
#include <sys/threads.h>
#include <sys/list.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <poll.h>

#include <libtty.h>

#include "posix/idtree.h"
#include "posixsrv_private.h"
#include "posixsrv.h"

//#define PTY_TRACE(str, ...) printf("posixsrv pty: " str "\n", ##__VA_ARGS__)
#define PTY_TRACE(str, ...)

#define MASTER_OPEN   (1 << 0)
#define SLAVE_LOCKED  (1 << 1)
#define SLAVE_OPEN    (1 << 2)
#define PTY_CLOSING   (1 << 3)


static handler_t pts_write_op, pts_read_op, pts_open_op, pts_close_op, pts_devctl_op, pts_getattr_op;
static handler_t ptm_write_op, ptm_read_op, ptm_close_op, ptm_devctl_op, ptm_getattr_op;
static handler_t ptmx_open_op;


static void ptm_destroy(object_t *o);
static void pts_destroy(object_t *o);


static operations_t pts_ops = {
	.handlers = { NULL },
	.open = pts_open_op,
	.close = pts_close_op,
	.read = pts_read_op,
	.write = pts_write_op,
	.getattr = pts_getattr_op,
	.devctl = pts_devctl_op,
	.release = pts_destroy,
};


static operations_t ptmx_ops = {
	.handlers = { NULL },
	.open = ptmx_open_op,
};


static operations_t ptm_ops = {
	.handlers = { NULL },
	.close = ptm_close_op,
	.read = ptm_read_op,
	.write = ptm_write_op,
	.getattr = ptm_getattr_op,
	.devctl = ptm_devctl_op,
	.release = ptm_destroy,
};


typedef struct {
	object_t master, slave;
	libtty_common_t tty;
	libtty_callbacks_t ops;
	unsigned state;

	unsigned open_count;
	handle_t mutex, cond;
} pty_t;


static inline pty_t *pty_master(object_t *master)
{
	return (void *)master - offsetof(pty_t, master);
}


static inline pty_t *pty_slave(object_t *slave)
{
	return (void *)slave - offsetof(pty_t, slave);
}


static void pts_destroy(object_t *o)
{
	PTY_TRACE("destroying slave (%d)", object_id(o));
	pty_t *pty = pty_slave(o);

	mutexLock(pty->mutex);
	pty->open_count--;
	mutexUnlock(pty->mutex);
	condSignal(pty->cond);
}


static void ptm_destroy(object_t *o)
{
	PTY_TRACE("destroying master %d", object_id(o));
	char buf[32];
	msg_t msg;
	int len;
	pty_t *pty = pty_master(o);

	len = snprintf(buf, sizeof(buf), "%d", object_id(&pty->slave));

	memset(&msg, 0, sizeof(msg));

	if (lookup("/dev/pts", NULL, &msg.i.ln.dir) == EOK) {
		msg.type = mtUnlink;

		msg.i.data = buf;
		msg.i.size = len + 1;

		msgSend(msg.i.ln.dir.port, &msg);

	}

	object_destroy(&pty->slave);
	object_put(&pty->slave);
	libtty_close(&pty->tty);

	mutexLock(pty->mutex);
	pty->state |= PTY_CLOSING;

	while (pty->open_count)
		condWait(pty->cond, pty->mutex, 0);
	mutexUnlock(pty->mutex);

	libtty_destroy(&pty->tty);

	resourceDestroy(pty->mutex);
	resourceDestroy(pty->cond);
	free(o);
}


static request_t *pts_write_op(object_t *o, request_t *r)
{
	PTY_TRACE("pts_write(%d)", object_id(o));
	pty_t *pty = pty_slave(o);
	r->msg.o.io.err = libtty_write(&pty->tty, r->msg.i.data, r->msg.i.size, r->msg.i.io.mode);
	return r;
}


static request_t *pts_read_op(object_t *o, request_t *r)
{
	PTY_TRACE("pts_read(%d)", object_id(o));
	pty_t *pty = pty_slave(o);
	r->msg.o.io.err = libtty_read(&pty->tty, r->msg.o.data, r->msg.o.size, r->msg.i.io.mode);
	return r;
}


static request_t *pts_open_op(object_t *o, request_t *r)
{
	PTY_TRACE("pts_open(%d)", object_id(o));
	pty_t *pty = pty_slave(o);

	if (pty->state & PTY_CLOSING) {
		r->msg.o.io.err = -EPIPE;
	}
	else if (0 && pty->state & (SLAVE_LOCKED | SLAVE_OPEN)) {
		r->msg.o.io.err = -EACCES;
	}
	else {
		mutexLock(pty->mutex);
		pty->open_count++;
		mutexUnlock(pty->mutex);

		pty->state |= SLAVE_OPEN;
		/* TODO: Cleanup */
		r->msg.o.io.err = EOK;
	}

	return r;
}


static request_t *pts_close_op(object_t *o, request_t *r)
{
	PTY_TRACE("pts_close(%d)", object_id(o));
	pty_t *pty = pty_slave(o);

	if (pty->state & SLAVE_OPEN) {
		r->msg.o.io.err = EOK;
		pty->state &= ~SLAVE_OPEN;

		mutexLock(pty->mutex);
		pty->open_count--;
		mutexUnlock(pty->mutex);
		condSignal(pty->cond);
	}
	else {
		r->msg.o.io.err = -EACCES;
	}

	return r;
}


static request_t *pts_devctl_op(object_t *o, request_t *r)
{
	const void *in, *out;
	long unsigned request;
	pty_t *pty = pty_slave(o);
	int err;

	in = ioctl_unpack(&r->msg, &request, NULL);
	err = libtty_ioctl(&pty->tty, request, in, &out);
	ioctl_setResponse(&r->msg, request, err, out);

	return r;
}


static request_t *ptm_write_op(object_t *o, request_t *r)
{
	PTY_TRACE("ptm_write(%d, %d)", object_id(o), r->msg.i.size);
	pty_t *pty = pty_master(o);
	size_t i;

	mutexLock(pty->mutex);
	for (i = 0; i < r->msg.i.size; ++i)
		libtty_putchar(&pty->tty, ((unsigned char *)r->msg.i.data)[i], NULL);
	mutexUnlock(pty->mutex);

	r->msg.o.io.err = i;
	return r;
}


static request_t *ptm_read_op(object_t *o, request_t *r)
{
	PTY_TRACE("ptm_read(%d)", object_id(o));
	pty_t *pty = pty_master(o);
	size_t i;

	mutexLock(pty->mutex);
	while (!(pty->state & PTY_CLOSING) && !libtty_txready(&pty->tty))
		condWait(pty->cond, pty->mutex, 0);

	if (!(pty->state & PTY_CLOSING)) {
		for (i = 0; i < r->msg.o.size && libtty_txready(&pty->tty); ++i)
			((unsigned char *)r->msg.o.data)[i] = libtty_getchar(&pty->tty, NULL);
	}
	else {
		i = -EINVAL;
	}
	mutexUnlock(pty->mutex);

	r->msg.o.io.err = i;
	return r;
}


static request_t *ptm_close_op(object_t *o, request_t *r)
{
	PTY_TRACE("ptm_close(%d)", object_id(o));

	pty_t *pty = pty_master(o);
	pty->state &= ~MASTER_OPEN;
	object_destroy(o);
	return r;
}


static request_t *pts_getattr_op(object_t *o, request_t *r)
{
	pty_t *pty = pty_slave(o);

	if (r->msg.i.attr.type != atPollStatus) {
		r->msg.o.attr.val = -EINVAL;
		return r;
	}

	r->msg.o.attr.val = libtty_poll_status(&pty->tty) & r->msg.i.attr.val;
	return r;
}


static request_t *ptm_getattr_op(object_t *o, request_t *r)
{
	pty_t *pty = pty_master(o);
	unsigned ev , rev = 0;

	if (r->msg.i.attr.type != atPollStatus) {
		r->msg.o.attr.val = -EINVAL;
		return r;
	}

	ev = r->msg.i.attr.val;

	if (ev & POLLIN && libtty_txready(&pty->tty))
		rev |= POLLIN;

	if (ev & POLLOUT)
		rev |= POLLOUT;

	r->msg.o.attr.val = rev;
	return r;
}


static request_t *ptm_devctl_op(object_t *o, request_t *r)
{
	PTY_TRACE("ptm_devctl(%d)", object_id(o));
	pty_t *pty = pty_master(o);
	int err = -EINVAL;
	unsigned long request;
	unsigned ptyid;
	const void *in_data, *out_data;

	in_data = ioctl_unpack(&r->msg, &request, NULL);

	PTY_TRACE("ptm_devctl request: %x", request);

	switch (request) {
	case TIOCGPTN: /* get pty number */
		ptyid = object_id(&pty->slave);
		out_data = &ptyid;
		err = EOK;
		break;

	case TIOCSPTLCK: /* (un)lock slave */
		if (!*((int *)in_data) && pty->state & SLAVE_LOCKED) {
			pty->state &= ~SLAVE_LOCKED;
			err = EOK;
		}
		else if (*((int *)in_data) && !(pty->state & SLAVE_LOCKED)) {
			pty->state |= SLAVE_LOCKED;
			err = EOK;
		}
		break;
	}

	ioctl_setResponse(&r->msg, request, err, out_data);
	return r;
}


void ptm_signalReady(void *arg)
{
	pty_t *pty = arg;
	mutexLock(pty->mutex);
	condBroadcast(pty->cond);
	mutexUnlock(pty->mutex);
}


#define PTS_NAME_PADDING "XXXXXXXXXX"

static int ptm_create(int *id)
{
	PTY_TRACE("create master/slave pair");

	pty_t *pty;
	oid_t oid;
	char path[] = "/dev/pts/" PTS_NAME_PADDING;

	if ((pty = malloc(sizeof(*pty))) == NULL)
		return -ENOMEM;

	pty->ops.arg = pty;

	pty->ops.set_baudrate = NULL;
	pty->ops.set_cflag = NULL;
	pty->ops.signal_txready = ptm_signalReady;

	mutexCreate(&pty->mutex);
	condCreate(&pty->cond);

	if (libtty_init(&pty->tty, &pty->ops, SIZE_PAGE) < 0) {
		free(pty);
		return -ENOMEM;
	}

	pty->state = MASTER_OPEN | SLAVE_LOCKED;
	pty->open_count = 0;

	object_create(&pty->master, &ptm_ops);
	object_create(&pty->slave, &pts_ops);

	*id = object_id(&pty->master);
	oid.port = srv_port();
	oid.id = object_id(&pty->slave);
	snprintf(path + sizeof("/dev/pts"), sizeof(PTS_NAME_PADDING), "%d", (int)oid.id);

	object_put(&pty->master);
	return create_dev(&oid, path);
}

#undef PTS_NAME_PADDING


static request_t *ptmx_open_op(object_t *ptmx, request_t *r)
{
	int id;

	if (!(r->msg.o.io.err = ptm_create(&id)))
		r->msg.o.io.err = id;

	return r;
}


int pty_init()
{
	object_t *o;
	int err;

	mkdir("/dev/pts", 0);

	if ((o = malloc(sizeof(*o))) == NULL)
		return -ENOMEM;

	object_create(o, &ptmx_ops);
	err = object_link(o, "/dev/ptmx");
	object_put(o);
	return err;
}
