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

#include "posix/idtree.h"

#include "posixsrv_private.h"
#include "posixsrv.h"

//#define PTY_TRACE(str, ...) printf("posixsrv pty: " str "\n", ##__VA_ARGS__)
#define PTY_TRACE(str, ...)

#define MASTER_OPEN   (1 << 0)
#define SLAVE_LOCKED  (1 << 1)
#define SLAVE_OPEN    (1 << 2)


static handler_t pts_write_op, pts_read_op, pts_open_op, pts_close_op, pts_devctl_op, pts_getattr_op;
static handler_t ptm_write_op, ptm_read_op, ptm_close_op, ptm_devctl_op, ptm_getattr_op;
static handler_t ptmx_open_op;


static void ptm_destroy(object_t *o);


static operations_t pts_ops = {
	.handlers = { NULL },
	.open = pts_open_op,
	.close = pts_close_op,
	.read = pts_read_op,
	.write = pts_write_op,
	.getattr = pts_getattr_op,
	.devctl = pts_devctl_op,
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
	.devctl = ptm_devctl_op,
	.getattr = ptm_getattr_op,
	.release = ptm_destroy,
};


typedef struct {
	object_t master, slave;

	char mode;
	char state;

	object_t *ptm_read, *ptm_write;

	struct winsize size;
	struct termios tios;
} pty_t;


static inline pty_t *pty_master(object_t *master)
{
	return (void *)master - offsetof(pty_t, master);
}


static inline pty_t *pty_slave(object_t *slave)
{
	return (void *)slave - offsetof(pty_t, slave);
}


static void ptm_destroy(object_t *o)
{
	PTY_TRACE("destroying master %d", object_id(o));
	char buf[128];
	msg_t msg;
	int len;
	pty_t *pty = pty_master(o);

	/* FIXME: cleanup, ugly */
	object_get(object_id(&pty->slave));
	object_destroy(&pty->slave);
	object_put(&pty->slave);

	object_destroy(pty->ptm_read);
	object_put(pty->ptm_read);

	object_destroy(pty->ptm_write);
	object_put(pty->ptm_write);

	len = snprintf(buf, sizeof(buf), "%d", object_id(&pty->slave));

	memset(&msg, 0, sizeof(msg));

	if (lookup("/dev/pts", NULL, &msg.i.ln.dir) == EOK) {
		msg.type = mtUnlink;

		msg.i.data = buf;
		msg.i.size = len + 1;

		msgSend(msg.i.ln.dir.port, &msg);
	}

	free(o);
}


static request_t *pts_write_op(object_t *o, request_t *r)
{
	PTY_TRACE("pts_write(%d)", object_id(o));
	pty_t *pty = pty_slave(o);
	return pty->ptm_read->operations->write(pty->ptm_read, r);
}


static request_t *pts_read_op(object_t *o, request_t *r)
{
	PTY_TRACE("pts_read(%d)", object_id(o));
	pty_t *pty = pty_slave(o);
	return pty->ptm_write->operations->read(pty->ptm_write, r);
}


static request_t *pts_open_op(object_t *o, request_t *r)
{
	PTY_TRACE("pts_open(%d)", object_id(o));
	pty_t *pty = pty_slave(o);

	if (0 && pty->state & (SLAVE_LOCKED | SLAVE_OPEN)) {
		r->msg.o.io.err = -EACCES;
	}
	else {
		pty->state |= SLAVE_OPEN;
		/* TODO: Cleanup */
		pipe_open((struct _pipe_t *)pty->ptm_write, O_RDONLY | O_NONBLOCK, r);
		pipe_open((struct _pipe_t *)pty->ptm_read, O_WRONLY | O_NONBLOCK, r);
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
	}
	else {
		r->msg.o.io.err = -EACCES;
	}

	return r;
}


static request_t *pts_devctl_op(object_t *o, request_t *r)
{
	return r;
}


static request_t *ptm_write_op(object_t *o, request_t *r)
{
	PTY_TRACE("ptm_write(%d, %d)", object_id(o), r->msg.i.size);
	pty_t *pty = pty_master(o);
	return pty->ptm_write->operations->write(pty->ptm_write, r);
}


static request_t *ptm_read_op(object_t *o, request_t *r)
{
	PTY_TRACE("ptm_read(%d)", object_id(o));
	pty_t *pty = pty_master(o);
	return pty->ptm_read->operations->read(pty->ptm_read, r);
}


static request_t *ptm_close_op(object_t *o, request_t *r)
{
	PTY_TRACE("ptm_close(%d)", object_id(o));

	pty_t *pty = pty_master(o);
	pty->state &= ~MASTER_OPEN;
	object_destroy(o);
	return r;
}


static request_t *ptm_getattr_op(object_t *o, request_t *r)
{
	pty_t *pty = pty_master(o);
	unsigned ev, rev = 0;

	if (r->msg.i.attr.type != atPollStatus) {
		r->msg.o.attr.val = -EINVAL;
		return r;
	}

	ev = r->msg.i.attr.val;

	if (ev & POLLIN && pipe_avail(pty->ptm_read))
		rev |= POLLIN;

	if (ev & POLLOUT && pipe_free(pty->ptm_write))
		rev |= POLLOUT;

	r->msg.o.attr.val = rev;
	return r;
}


static request_t *pts_getattr_op(object_t *o, request_t *r)
{
	pty_t *pty = pty_slave(o);
	unsigned ev, rev = 0;

	if (r->msg.i.attr.type != atPollStatus) {
		r->msg.o.attr.val = -EINVAL;
		return r;
	}

	ev = r->msg.i.attr.val;

	if (ev & POLLIN && pipe_avail(pty->ptm_write))
		rev |= POLLIN;

	if (ev & POLLOUT && pipe_free(pty->ptm_read))
		rev |= POLLOUT;

	r->msg.o.attr.val = rev;
	return r;
}


static request_t *ptm_devctl_op(object_t *o, request_t *r)
{
	PTY_TRACE("ptm_devctl(%d)", object_id(o));

	posixsrv_devctl_t *devctl = (void *)r->msg.i.raw;
	pty_t *pty = pty_master(o);
	int err = -EINVAL;

	switch (devctl->type) {
	case pxUnlockpt:
		if (pty->state & SLAVE_LOCKED) {
			pty->state &= ~SLAVE_LOCKED;
			err = EOK;
		}
		break;
	case pxGrantpt:
		err = EOK;
		break;
	case pxPtsname:
		if (snprintf(r->msg.o.data, r->msg.o.size, "/dev/pts/%d", object_id(&pty->slave)) > r->msg.o.size)
			err = -ERANGE;
		else
			err = EOK;
		break;
	}

	r->msg.o.io.err = err;

	return r;
}


#define PTS_NAME_PADDING "XXXXXXXXXX"

static int ptm_create(int *id)
{
	PTY_TRACE("create master/slave pair");

	int p[2];
	pty_t *pty;
	oid_t oid;
	char path[] = "/dev/pts/" PTS_NAME_PADDING;

	if (pipe_create(pxBufferedPipe, p, O_RDONLY) < 0)
		return -ENOMEM;

	if (pipe_create(pxBufferedPipe, p + 1, O_WRONLY) < 0)
		return -ENOMEM;

	if ((pty = malloc(sizeof(*pty))) == NULL)
		return -ENOMEM;

	pty->ptm_read = object_get(p[0]);
	pty->ptm_write = object_get(p[1]);
	pty->state = MASTER_OPEN | SLAVE_LOCKED;

	object_create(&pty->master, &ptm_ops);
	object_create(&pty->slave, &pts_ops);

	*id = object_id(&pty->master);
	oid.port = srv_port();
	oid.id = object_id(&pty->slave);
	snprintf(path + sizeof("/dev/pts"), sizeof(PTS_NAME_PADDING), "%d", (int)oid.id);

	object_put(&pty->slave);
	object_put(&pty->master);

	create_dev(&oid, path);

	return EOK;
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
