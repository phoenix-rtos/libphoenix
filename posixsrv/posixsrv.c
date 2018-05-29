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

#include ARCH

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

#include "posix/idtree.h"

#define PIPE_BUFSZ 0x1000


typedef struct request_t {
	struct request_t *next, *prev;

	unsigned int rid;
	msg_t msg;
} request_t;


typedef struct {
	idnode_t linkage;

	void *buf;
	unsigned r, w, full;
	handle_t mutex;
	int rrefs, wrefs;

	request_t *queue;
} pipe_t;


struct {
	unsigned port;
	idtree_t pipes;
	handle_t pipesLock;
} posixsrv_common;


static inline int rq_id(request_t *r)
{
	return r->msg.i.io.oid.id >> 1;
}


static inline int rq_sz(request_t *r)
{
	return (r->msg.type == mtWrite) ? r->msg.i.size : r->msg.o.size;
}


static inline void *rq_buf(request_t *r)
{
	return (r->msg.type == mtWrite) ? r->msg.i.data : r->msg.o.data;
}


static void fail(char *err)
{
	printf("posixsrv: %s\n", err);
	exit(1);
}


static void _pipe_wakeup(pipe_t *p, request_t *r, int retval)
{
	LIST_REMOVE(&p->queue, r);
	r->msg.o.io.err = retval;
	msgRespond(posixsrv_common.port, &r->msg, r->rid);
	free(r);
}


static int pipe_create(oid_t *oid)
{
	pipe_t *p;

	if ((p = malloc(sizeof(pipe_t))) == NULL)
		return -ENOMEM;

	if (PIPE_BUFSZ & (SIZE_PAGE - 1))
		p->buf = malloc(PIPE_BUFSZ);
	else
		p->buf = mmap(NULL, PIPE_BUFSZ, PROT_READ | PROT_WRITE, MAP_NONE, OID_NULL, 0);

	if (p->buf == NULL || p->buf == (void *)-1) {
		free(p);
		return -ENOMEM;
	}

	p->r = p->w = p->full = 0;
	p->rrefs = p->wrefs = 1;
	p->queue = NULL;
	mutexCreate(&p->mutex);

	mutexLock(posixsrv_common.pipesLock);
	idtree_alloc(&posixsrv_common.pipes, &p->linkage);
	mutexUnlock(posixsrv_common.pipesLock);

	oid->port = posixsrv_common.port;
	oid->id = idtree_id(&p->linkage) << 1;

	return EOK;
}


static void pipe_destroy(pipe_t *p)
{
	mutexLock(posixsrv_common.pipesLock);
	idtree_remove(&posixsrv_common.pipes, &p->linkage);
	mutexUnlock(posixsrv_common.pipesLock);

	resourceDestroy(p->mutex);
	munmap(p->buf, PIPE_BUFSZ);
	free(p);
}


static pipe_t *pipe_find(int id)
{
	pipe_t *p;

	mutexLock(posixsrv_common.pipesLock);
	p = lib_treeof(pipe_t, linkage, (void *)idtree_find(&posixsrv_common.pipes, id));
	mutexUnlock(posixsrv_common.pipesLock);

	return p;
}


static int _pipe_write(pipe_t *p, void *buf, size_t sz)
{
	int bytes = 0;

	if (p->r == p->w && p->full)
		return bytes;

	if (p->r > p->w) {
		memcpy(p->buf + p->w, buf, bytes = min(sz, p->r - p->w));
	}
	else {
		memcpy(p->buf + p->w, buf, bytes = min(sz, PIPE_BUFSZ - p->w));

		if (bytes < sz && p->r) {
			buf += bytes;
			sz -= bytes;
			memcpy(p->buf, buf, min(sz, p->r));
			bytes += min(sz, p->r);
		}
	}

	p->w = (p->w + bytes) % PIPE_BUFSZ;

	if (p->w == p->r)
		p->full = 1;

	return bytes;
}


static int _pipe_read(pipe_t *p, void *buf, size_t sz)
{
	int bytes = 0;

	if (p->r == p->w && !p->full)
		return bytes;

	if (p->w > p->r) {
		memcpy(buf, p->buf + p->r, bytes = min(sz, p->w - p->r));
	}
	else {
		memcpy(buf, p->buf + p->r, bytes = min(sz, PIPE_BUFSZ - p->r));

		if (bytes < sz) {
			buf += bytes;
			sz -= bytes;
			memcpy(buf, p->buf, min(sz, p->w));
			bytes += min(sz, p->w);
		}
	}

	p->r = (p->r + bytes) % PIPE_BUFSZ;

	if (p->w == p->r)
		p->full = 0;

	return bytes;
}


static int pipe_write(request_t *r)
{
	pipe_t *p;
	int sz = rq_sz(r), bytes = 0, c;
	void *buf = rq_buf(r);

	if ((p = pipe_find(rq_id(r))) == NULL)
		return -EINVAL;

	mutexLock(p->mutex);

	if (p->rrefs) {
		/* write to pending readers */
		while (p->queue != NULL && !p->full && bytes < sz) {
			memcpy(rq_buf(p->queue), buf + bytes, c = min(sz - bytes, rq_sz(p->queue)));
			_pipe_wakeup(p, p->queue, c);
			bytes += c;
		}

		/* write to buffer */
		if (!(bytes += _pipe_write(p, buf + bytes, sz - bytes)))
			LIST_ADD(&p->queue, r);
	}
	else {
		bytes = -EPIPE;
	}

	mutexUnlock(p->mutex);

	return bytes;
}


static int pipe_read(request_t *r)
{
	pipe_t *p;
	int sz = rq_sz(r), bytes = 0, c, was_full;
	void *buf = rq_buf(r);

	if ((p = pipe_find(rq_id(r))) == NULL)
		return -EINVAL;

	mutexLock(p->mutex);

	if (p->wrefs) {
		/* read from buffer */
		was_full = p->full;
		bytes += _pipe_read(p, buf, sz);

		if (was_full) {
			/* read from pending writers */
			while (p->queue != NULL && bytes < sz) {
				memcpy(buf + bytes, rq_buf(p->queue), c = min(sz - bytes, rq_sz(p->queue)));
				_pipe_wakeup(p, p->queue, c);
				bytes += c;
			}

			/* discharge remaining pending writers */
			while (p->queue != NULL && (c = _pipe_write(p, rq_buf(p->queue), rq_sz(p->queue))))
				_pipe_wakeup(p, p->queue, c);
		}

		if (!bytes)
			LIST_ADD(&p->queue, r);
	}
	else {
		bytes = -EPIPE;
	}

	mutexUnlock(p->mutex);

	return bytes;
}


static int pipe_open(int id, int w)
{
	pipe_t *p;

	if ((p = pipe_find(id)) == NULL)
		return -EINVAL;

	mutexLock(p->mutex);
	if (w)
		p->wrefs++;
	else
		p->rrefs++;
	mutexUnlock(p->mutex);

	return 0;
}


static int pipe_close(int id, int w)
{
	pipe_t *p;

	if ((p = pipe_find(id)) == NULL)
		return -EINVAL;

	mutexLock(p->mutex);
	if (w) {
		if (!p->wrefs) {
			mutexUnlock(p->mutex);
			return -EINVAL;
		}

		p->wrefs--;

		if (!p->wrefs) {
			while (p->queue != NULL)
				_pipe_wakeup(p, p->queue, 0);
		}
	}
	else {
		if (!p->rrefs) {
			mutexUnlock(p->mutex);
			return -EINVAL;
		}

		p->rrefs--;

		if (!p->wrefs && p->queue != NULL) {
			while (p->queue != NULL)
				_pipe_wakeup(p, p->queue, -EPIPE);
		}
	}

	if (!p->wrefs && !p->rrefs) {
		mutexUnlock(p->mutex);
		pipe_destroy(p);
		return EOK;
	}

	mutexUnlock(p->mutex);

	return EOK;
}


int main(int argc, char **argv)
{
	msg_t msg;
	oid_t oid = {0}, fs;
	request_t *r = NULL;
	int id, tag;

	mutexCreate(&posixsrv_common.pipesLock);
	idtree_init(&posixsrv_common.pipes);

	while (lookup("/", &fs) < 0)
		usleep(5000);

	if (portCreate(&posixsrv_common.port) < 0)
		fail("port create");

	msg.type = mtCreate;
	msg.i.create.type = otDev;
	msg.i.create.port = posixsrv_common.port;
	msg.i.create.mode = 0;

	msg.i.data = NULL;
	msg.i.size = 0;

	msg.o.data = NULL;
	msg.o.size = 0;

	if (msgSend(fs.port, &msg) < 0 || msg.o.io.err < 0)
		fail("create dev in filesystem");

	oid = msg.o.create.oid;

	msg.type = mtLink;
	msg.i.ln.dir = fs;
	msg.i.ln.oid = oid;
	msg.i.data = "posix";
	msg.i.size = sizeof("posix");

	if (msgSend(fs.port, &msg) < 0 || msg.o.io.err < 0)
		fail("link in filesystem");

	for (;;) {
		if (r == NULL)
			r = malloc(sizeof(*r));

		if (msgRecv(posixsrv_common.port, &r->msg, &r->rid) < 0) {
			msgRespond(posixsrv_common.port, &r->msg, r->rid);
			continue;
		}

		id = r->msg.i.openclose.oid.id >> 1;
		tag = r->msg.i.openclose.oid.id & 1;

		switch (r->msg.type) {
		case mtCreate:
			r->msg.o.create.err = pipe_create(&r->msg.o.create.oid);
			break;
		case mtRead:
			if (!(r->msg.o.io.err = pipe_read(r))) {
				r = NULL;
				continue;
			}

			if (r->msg.o.io.err == -EPIPE)
				r->msg.o.io.err = 0;

			break;
		case mtWrite:
			if (!(r->msg.o.io.err = pipe_write(r))) {
				r = NULL;
				continue;
			}
			break;
		case mtOpen:
			r->msg.o.io.err = pipe_open(id, tag);
			break;
		case mtClose:
			r->msg.o.io.err = pipe_close(id, tag);
			break;
		default:
			r->msg.o.io.err = -EINVAL;
		}

		msgRespond(posixsrv_common.port, &r->msg, r->rid);
	}
}
