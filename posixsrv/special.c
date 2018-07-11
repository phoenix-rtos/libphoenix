/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * POSIX server - /dev/{zero,null}
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
#include <errno.h>
#include <string.h>
#include <fcntl.h>

#include "posixsrv.h"
#include "posixsrv_private.h"


static request_t *nothing_op(object_t *o, request_t *r)
{
	return r;
}


static request_t *null_read_op(object_t *o, request_t *r)
{
	r->msg.o.io.err = 0;
	return r;
}


static request_t *null_write_op(object_t *o, request_t *r)
{
	r->msg.o.io.err = r->msg.i.size;
	return r;
}


static request_t *zero_read_op(object_t *o, request_t *r)
{
	memset(r->msg.o.data, 0, r->msg.o.size);
	r->msg.o.io.err = r->msg.o.size;
	return r;
}


static operations_t null_ops = {
	.handlers = { NULL },
	.open = nothing_op,
	.close = nothing_op,
	.read = null_read_op,
	.write = null_write_op,
	.release = (void *)free,
};


static operations_t zero_ops = {
	.handlers = { NULL },
	.open = nothing_op,
	.close = nothing_op,
	.read = zero_read_op,
	.write = null_write_op,
	.release = (void *)free,
};


int special_init()
{
	object_t *o;
	int err;

	if ((o = malloc(sizeof(*o))) == NULL)
		return -ENOMEM;

	object_create(o, &null_ops);
	err = object_link(o, "/dev/null");
	object_put(o);

	if ((o = malloc(sizeof(*o))) == NULL)
		return -ENOMEM;

	object_create(o, &zero_ops);
	err = object_link(o, "/dev/zero");
	object_put(o);

	return err;
}
