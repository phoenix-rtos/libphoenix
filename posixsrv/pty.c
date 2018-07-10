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

#include "posix/idtree.h"

#include "posixsrv_private.h"
#include "posixsrv.h"

#define PTY_TRACE(str, ...) printf("posixsrv pty: " str "\n", ##__VA_ARGS__)
//#define PTY_TRACE(str, ...)

#define MASTER_OPEN   (1 << 0)
#define SLAVE_LOCKED  (1 << 1)
#define SLAVE_OPEN    (1 << 2)


static handler_t pts_write_op, pts_read_op, pts_open_op, pts_close_op, pts_devctl_op;
static handler_t ptm_write_op, ptm_read_op, ptm_close_op, ptm_devctl_op;
static handler_t ptmx_open_op;


static void ptm_destroy(object_t *o);


static operations_t pts_ops = {
	.handlers = { NULL },
	.open = pts_open_op,
	.close = pts_close_op,
	.read = pts_read_op,
	.write = pts_write_op,
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
	pty_t *pty = pty_master(o);

	object_destroy(pty->ptm_read);
	object_put(pty->ptm_read);

	object_destroy(pty->ptm_write);
	object_put(pty->ptm_write);

	free(o);
}


static request_t *pts_write_op(object_t *o, request_t *r)
{
	pty_t *pty = pty_slave(o);
	return pty->ptm_read->operations->write(o, r);
}


static request_t *pts_read_op(object_t *o, request_t *r)
{
	pty_t *pty = pty_slave(o);
	return pty->ptm_write->operations->read(o, r);
}


static request_t *pts_open_op(object_t *o, request_t *r)
{
	pty_t *pty = pty_slave(o);

	if (pty->state & (SLAVE_LOCKED | SLAVE_OPEN)) {
		r->msg.o.io.err = -EACCES;
	}
	else {
		pty->state |= SLAVE_OPEN;
		r->msg.o.io.err = EOK;
	}

	return r;
}


static request_t *pts_close_op(object_t *o, request_t *r)
{
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
	pty_t *pty = pty_master(o);
	return pty->ptm_write->operations->write(o, r);
}


static request_t *ptm_read_op(object_t *o, request_t *r)
{
	pty_t *pty = pty_master(o);
	return pty->ptm_read->operations->read(o, r);
}


static request_t *ptm_close_op(object_t *o, request_t *r)
{
	PTY_TRACE("close master");

	pty_t *pty = pty_master(o);
	char path[] = "/dev/pts/XXXXXXXXXX";

	sprintf(path + sizeof("/dev/pts"), "%d", object_id(o));
	pty->state &= ~MASTER_OPEN;
	unlink(path);

	object_destroy(o);
	return r;
}


static request_t *ptm_devctl_op(object_t *o, request_t *r)
{
	posixsrv_devctl_t *devctl = (void *)r->msg.i.raw;
	pty_t *pty = pty_master(o);
	int err = -EINVAL;

	if (devctl->pty.type == pxUnlockpt) {
		if (pty->state & SLAVE_LOCKED) {
			pty->state &= ~SLAVE_LOCKED;
			err = EOK;
		}
	}

	r->msg.o.io.err = err;

	return r;
}


static int ptm_create(int *id)
{
	PTY_TRACE("create master/slave pair");

	int p[2];
	pty_t *pty;
	oid_t oid;
	char path[] = "/dev/pts/XXXXXXXXXX";

	if (pipe_create(pxPipe, p, O_RDONLY) < 0)
		return -ENOMEM;

	if (pipe_create(pxPipe, p + 1, O_WRONLY) < 0)
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
	sprintf(path + sizeof("/dev/pts"), "%d", (int)oid.id);

	object_put(&pty->slave);
	object_put(&pty->master);

	create_dev(&oid, path);

	return EOK;
}


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
