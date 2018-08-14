/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * POSIX implementation - server
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

#include "posix/idtree.h"

#include "posixsrv_private.h"
#include "posixsrv.h"

//#define TRACE(str, ...) printf("posixsrv: " str "\n", ##__VA_ARGS__)
#define TRACE(str, ...)


struct {
	unsigned port;

	handle_t lock;
	idtree_t objects;

	struct {
		int id;
		object_t *o;
	} cache;

	char stacks[8][0x1000] __attribute__ ((aligned(8)));
} posixsrv_common;


static int fail(const char *str)
{
	printf("posixsrv fail: %s\n", str);
	exit(EXIT_FAILURE);
}


void object_destroy(object_t *o)
{
	o->destroy = 1;
}


object_t *object_get(int id)
{
	object_t *o;

	while (mutexLock(posixsrv_common.lock) < 0);

	if (posixsrv_common.cache.id == id)
		o = posixsrv_common.cache.o;
	else
		o = lib_treeof(object_t, linkage, (void *)idtree_find(&posixsrv_common.objects, id));

	if (o != NULL) {
		if (o->destroy) {
			o = NULL;
		}
		else {
			posixsrv_common.cache.id = id;
			posixsrv_common.cache.o = o;
			o->refs++;
		}
	}

	mutexUnlock(posixsrv_common.lock);

	return o;
}


void object_put(object_t *o)
{
	while (mutexLock(posixsrv_common.lock) < 0);

	if (!--o->refs && o->destroy) {
		TRACE("removing %d", o->linkage.id);

		if (posixsrv_common.cache.id == o->linkage.id)
			posixsrv_common.cache.o = NULL;

		idtree_remove(&posixsrv_common.objects, &o->linkage);
		mutexUnlock(posixsrv_common.lock);

		if (o->operations->release != NULL)
			o->operations->release(o);

		return;
	}

	mutexUnlock(posixsrv_common.lock);
	return;
}


int object_create(object_t *o, operations_t *ops)
{
	o->destroy = 0;
	o->operations = ops;
	o->refs = 1;

	while (mutexLock(posixsrv_common.lock) < 0);
	idtree_alloc(&posixsrv_common.objects, &o->linkage);
	posixsrv_common.cache.id = o->linkage.id;
	posixsrv_common.cache.o = o;
	mutexUnlock(posixsrv_common.lock);

	TRACE("created %d", o->linkage.id);

	return EOK;
}


int object_link(object_t *o, char *path)
{
	oid_t oid;
	TRACE("linking %d to %s", o->linkage.id, path);

	oid.port = posixsrv_common.port;
	oid.id = o->linkage.id;

	return create_dev(&oid, path);
}



unsigned srv_port(void)
{
	return posixsrv_common.port;
}


void rq_wakeup(request_t *r, int retval)
{
	r->msg.o.io.err = retval;
	msgRespond(posixsrv_common.port, &r->msg, r->rid);
	free(r);
}


int rq_id(request_t *r)
{
	int id;
	id_t full_id;

	switch (r->msg.type) {
	case mtOpen:
	case mtClose:
		id = r->msg.i.openclose.oid.id;
		break;

	case mtRead:
	case mtWrite:
	case mtTruncate:
		id = r->msg.i.io.oid.id;
		break;

	case mtCreate:
		id = r->msg.i.create.dir.id;
		break;

	case mtDestroy:
		id = r->msg.i.destroy.oid.id;
		break;

	case mtSetAttr:
	case mtGetAttr:
		id = r->msg.i.attr.oid.id;
		break;

	case mtLookup:
		id = r->msg.i.lookup.dir.id;
		break;

	case mtLink:
	case mtUnlink:
		id = r->msg.i.ln.oid.id;
		break;

	case mtReaddir:
		id = r->msg.i.readdir.dir.id;
		break;

	case mtDevCtl:
		ioctl_unpack(&r->msg, NULL, &full_id);
		id = (int)full_id;
		break;

	default:
		id = -1;
		break;
	}

	return id;
}


void posixsrvthr(void *arg)
{
	object_t *o;
	request_t *r = NULL;

	for (;;) {
		if (r == NULL)
			r = malloc(sizeof(*r));

		if (msgRecv(posixsrv_common.port, &r->msg, &r->rid) < 0)
			continue;

		if ((o = object_get(rq_id(r))) == NULL || o->operations->handlers[r->msg.type] == NULL) {
			if (o != NULL)
				object_put(o);
			r->msg.o.io.err = -EINVAL;
			msgRespond(posixsrv_common.port, &r->msg, r->rid);
			continue;
		}

		if ((r = o->operations->handlers[r->msg.type](o, r)) != NULL)
			msgRespond(posixsrv_common.port, &r->msg, r->rid);

		object_put(o);
	}
}


int main(int argc, char **argv)
{
	oid_t fs;
	int i;

	idtree_init(&posixsrv_common.objects);
	mutexCreate(&posixsrv_common.lock);

	while (lookup("/", NULL, &fs) < 0)
		usleep(5000);

	if (portCreate(&posixsrv_common.port) < 0)
		fail("port create");

	mkdir("/dev", 0);
	mkdir("/dev/posix", 0);

	if (special_init() < 0)
		fail("special init");

	if (event_init() < 0)
		fail("event init");

	if (pipe_init() < 0)
		fail("pipe init");

	if (pty_init() < 0)
		fail("pty init");

	for (i = 0; i < sizeof(posixsrv_common.stacks) / sizeof(posixsrv_common.stacks[0]); ++i)
		beginthread(posixsrvthr, 4, posixsrv_common.stacks[i], sizeof(posixsrv_common.stacks[i]), NULL);

	posixsrvthr(NULL);
	return 0;
}
