/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * POSIX implementation - events (poll/select)
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <sys/msg.h>
#include <sys/file.h>
#include <sys/threads.h>
#include <sys/rb.h>
#include <sys/list.h>
#include <sys/stat.h>

#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "posix/idtree.h"
#include "posixsrv_private.h"
#include "posixsrv.h"


#define INITIAL_EVENT_BUF_COUNT 16
#define GROW_EVENT_BUF(sz) (2 * (sz))


typedef struct _evqueue_t {
	struct _evqueue_t *next, *prev;

	object_t object;
	handle_t lock;
	request_t *requests;
	struct _evnote_t *notes;
} evqueue_t;


typedef struct _evnote_t {
	struct _eventry_t *entry;
	struct _evnote_t *next, *prev;
	struct _evqueue_t *queue;
	struct _evnote_t *queue_next, *queue_prev;

	unsigned short mask;
	unsigned short pend;
	unsigned short enabled;
	unsigned short oneshot;
	unsigned short dispatch;

	struct {
		unsigned flags;
		unsigned count;
		unsigned data;
	} pending[16];
} evnote_t;


typedef struct _eventry_t {
	rbnode_t node;
	oid_t oid;
	handle_t lock;
	unsigned refs;

	unsigned short mask;
	evnote_t *notes;
} eventry_t;


static handler_t sink_create_op, sink_write_op, sink_open_op, sink_close_op;
static handler_t queue_read_op, queue_write_op, queue_close_op /*, queue_devctl_op*/;
static handler_t qmx_open_op;


static operations_t sink_ops = {
	.handlers = { NULL },
	.open = sink_open_op,
	.close = sink_close_op,
	.write = sink_write_op,
	.create = sink_create_op,
};


static void queue_timeout_op(request_t *r);
static void queue_destroy(object_t *o);

static operations_t queue_ops = {
	.handlers = { NULL },
	.close = queue_close_op,
	.write = queue_write_op,
	.read = queue_read_op,
	/* .devctl = queue_devctl_op, */
	.timeout = queue_timeout_op,
	.release = queue_destroy,
};


static operations_t qmx_ops = {
	.handlers = { NULL },
	.open = qmx_open_op,
};


static struct {
	object_t sink;
	object_t qmx;
	handle_t lock;
	rbtree_t notes;
} event_common;


static inline evqueue_t *evqueue(object_t *o)
{
	return (evqueue_t *)((char *)o - offsetof(evqueue_t, object));
}


static int event_cmp(rbnode_t *n1, rbnode_t *n2)
{
	eventry_t *e1 = lib_treeof(eventry_t, node, n1);
	eventry_t *e2 = lib_treeof(eventry_t, node, n2);

	if (e1->oid.port != e2->oid.port)
		return e1->oid.port > e2->oid.port ? 1 : -1;

	if (e1->oid.id != e2->oid.id)
		return e1->oid.id > e2->oid.id ? 1 : -1;

	return 0;
}


static void queue_add(evqueue_t *queue, evqueue_t **wakeq)
{
	mutexLock(event_common.lock);
	if (queue->next == NULL) {
		object_ref(&queue->object);
		LIST_ADD(wakeq, queue);
	}
	mutexUnlock(event_common.lock);
}


static eventry_t *_entry_find(oid_t *oid)
{
	eventry_t find, *entry;
	memcpy(&find.oid, oid, sizeof(oid_t));
	if ((entry = lib_treeof(eventry_t, node, lib_rbFind(&event_common.notes, &find.node))) != NULL)
		entry->refs++;
	return entry;
}


static void entry_ref(eventry_t *entry)
{
	mutexLock(event_common.lock);
	++entry->refs;
	mutexUnlock(event_common.lock);
}


static eventry_t *entry_find(oid_t *oid)
{
	eventry_t *entry;

	mutexLock(event_common.lock);
	entry = _entry_find(oid);
	mutexUnlock(event_common.lock);
	return entry;
}


static void _entry_remove(eventry_t *entry)
{
	resourceDestroy(entry->lock);
	lib_rbRemove(&event_common.notes, &entry->node);
	free(entry);
}


static eventry_t *_entry_new(oid_t *oid)
{
	eventry_t *entry;

	if ((entry = calloc(1, sizeof(eventry_t))) == NULL)
		return NULL;

	memcpy(&entry->oid, oid, sizeof(oid_t));
	mutexCreate(&entry->lock);
	entry->refs = 1;
	lib_rbInsert(&event_common.notes, &entry->node);
	return entry;
}


static eventry_t *entry_get(oid_t *oid)
{
	eventry_t *entry;

	mutexLock(event_common.lock);
	if ((entry = _entry_find(oid)) == NULL)
		entry = _entry_new(oid);
	mutexUnlock(event_common.lock);
	return entry;
}


static void entry_put(eventry_t *entry)
{
	mutexLock(event_common.lock);
	if (!--entry->refs)
		_entry_remove(entry);
	mutexUnlock(event_common.lock);
}


static void _entry_register(eventry_t *entry, event_t *event, evqueue_t **wakeq)
{
	evnote_t *note;
	unsigned short typebit;

	typebit = 1 << event->type;

	if (!(entry->mask & typebit))
		return;

	note = entry->notes;
	do {
		if (note->mask & typebit) {
			if (note->pend & typebit) {
				note->pending[event->type].flags |= event->flags;
				note->pending[event->type].count += event->count;
			}
			else {
				note->pend |= typebit;
				note->pending[event->type].flags = event->flags;
				note->pending[event->type].count = event->count;

				queue_add(note->queue, wakeq);
			}

			note->pending[event->type].data = event->data;
		}

		note = note->next;
	} while (note != entry->notes);
}


static void _entry_notify(eventry_t *entry)
{
	msg_t msg;

	msg.type = mtSetAttr;
	msg.i.attr.type = atEventMask;
	memcpy(&msg.i.attr.oid, &entry->oid, sizeof(oid_t));
	msg.i.attr.val = entry->mask;

	msg.i.data = msg.o.data = NULL;
	msg.i.size = msg.o.size = 0;

	msgSend(entry->oid.port, &msg);
}


static void _note_poll(evnote_t *note)
{
	/* TODO: only poll events known to be level triggered? */
	msg_t msg;

	msg.type = mtGetAttr;
	memcpy(&msg.i.attr.oid, &note->entry->oid, sizeof(oid_t));
	msg.i.attr.type = atPollStatus;

	msg.i.data = msg.o.data = NULL;
	msg.i.size = msg.o.size = 0;

	/* TODO: have a way to update event data */

	if (msgSend(note->entry->oid.port, &msg) == EOK && msg.o.attr.val > 0)
		note->pend |= msg.o.attr.val & note->mask;
}


static void _entry_recalculate(eventry_t *entry)
{
	evnote_t *note;
	unsigned short mask = 0, oldmask;

	if ((note = entry->notes) != NULL) {
		do {
			mask |= note->mask;
			note = note->next;
		} while (note != entry->notes);
	}

	oldmask = entry->mask;
	entry->mask = mask;

	if (mask != oldmask)
		_entry_notify(entry);
}


static evnote_t *_note_new(evqueue_t *queue, eventry_t *entry)
{
	evnote_t *note;

	if ((note = calloc(1, sizeof(evnote_t))) == NULL)
		return NULL;

	note->entry = entry;

	object_ref(&queue->object);
	note->queue = queue;

	LIST_ADD(&entry->notes, note);
	LIST_ADD_EX(&queue->notes, note, queue_next, queue_prev);

	return note;
}


static void _note_remove(evnote_t *note)
{
	LIST_REMOVE(&note->entry->notes, note);
	entry_put(note->entry);
	object_put(&note->queue->object);

	LIST_REMOVE_EX(&note->queue->notes, note, queue_next, queue_prev);
	free(note);
}


static void _note_merge(evnote_t *note, evsub_t *sub)
{
	if (sub->flags & evAdd) {
		note->mask    |= sub->types;
		note->enabled |= sub->types;
	}

	if (sub->flags & evDelete) {
		note->pend     &= ~sub->types;
		note->mask     &= ~sub->types;
		note->enabled  &= ~sub->types;
		note->oneshot  &= ~sub->types;
		note->dispatch &= ~sub->types;
	}

	if (sub->flags & evEnable)
		note->enabled |= sub->types;

	if (sub->flags & evDisable)
		note->enabled &= ~sub->types;

	if (sub->flags & evOneshot)
		note->oneshot |= sub->types;

	if (sub->flags & evDispatch)
		note->dispatch |= sub->types;

	if (sub->flags & evClear)
		note->pend &= ~sub->types;
}


static int _event_subscribe(evqueue_t *queue, evsub_t *sub, int count)
{
	evnote_t *note;
	eventry_t *entry;
	unsigned short mask;

	while (count--) {
		if ((note = queue->notes) != NULL) {
			do {
				entry = note->entry;
				if (!memcmp(&entry->oid, &sub->oid, sizeof(oid_t))) {
					mutexLock(entry->lock);
					goto got_note;
				}
				note = note->queue_next;
			} while (note != queue->notes);
		}

		/* this reference is donated to the new note created below */
		if ((entry = entry_get(&sub->oid)) == NULL)
			return -ENOMEM;

		/* we keep one more reference in case the note gets removed */
		entry_ref(entry);
		mutexLock(entry->lock);

		if ((note = _note_new(queue, entry)) == NULL) {
			mutexUnlock(entry->lock);
			entry_put(entry);
			return -ENOMEM;
		}

	got_note:
		mask = note->mask;
		_note_merge(note, sub);

		if (note->mask != mask) {
			if (mask & ~note->mask) {
				/* change might clear some bits */
				_entry_recalculate(entry);
			}
			else if ((entry->mask & note->mask) != note->mask) {
				entry->mask |= note->mask;
				_entry_notify(entry);
			}
		}

		if (!note->mask)
			_note_remove(note);

		mutexUnlock(entry->lock);
		entry_put(entry);
		sub++;
	}

	return EOK;
}


static void queue_wakeup(evqueue_t *queue);


void event_register(event_t *events, int count)
{
	event_t *event;
	eventry_t *entry;
	evqueue_t *wakeq = NULL;
	int i = 0;

	for (i = 0; i < count; ++i) {
		event = events + i;

		if ((entry = entry_find(&event->oid)) == NULL)
			continue;

		mutexLock(entry->lock);
		_entry_register(entry, event, &wakeq);
		mutexUnlock(entry->lock);

		entry_put(entry);
	}

	queue_wakeup(wakeq);
}


static evqueue_t *queue_create(void)
{
	evqueue_t *queue;

	if ((queue = calloc(1, sizeof(evqueue_t))) == NULL)
		return NULL;

	if (mutexCreate(&queue->lock) < 0) {
		free(queue);
		return NULL;
	}

	object_create(&queue->object, &queue_ops);
	object_put(&queue->object);
	return queue;
}


static void queue_destroy(object_t *o)
{
	evqueue_t *queue = evqueue(o);

	if (queue->notes != NULL || queue->requests != NULL)
		printf("posixsrv/event.c error: destroying busy queue\n");

	resourceDestroy(queue->lock);
	free(queue);
}


static int _event_read(evqueue_t *queue, event_t *event, int eventcnt)
{
	int type, i = 0;
	unsigned short typebit;
	evnote_t *note;

	if ((note = queue->notes) == NULL)
		return 0;

	do {
		mutexLock(note->entry->lock);
		for (type = 0; type < sizeof(note->pending) / sizeof(*note->pending) && i < eventcnt; ++type) {
			typebit = 1 << type;

			if (note->pend & note->mask & note->enabled & typebit) {
				memcpy(&event->oid, &note->entry->oid, sizeof(oid_t));
				event->type = type;
				event->flags = note->pending[type].flags;
				event->count = note->pending[type].count;
				event->data = note->pending[type].data;

				if (note->oneshot & typebit) {
					note->mask &= ~typebit;
					_entry_recalculate(note->entry);
				}

				if (note->dispatch & typebit)
					note->enabled &= ~typebit;

				++i;
				++event;
				note->pend &= ~typebit;
			}
		}
		mutexUnlock(note->entry->lock);

		note = note->queue_next;
	} while (note != queue->notes && i < eventcnt);

	return i;
}


static void _queue_poll(evqueue_t *queue)
{
	evnote_t *note;

	if ((note = queue->notes) == NULL)
		return;

	do {
		mutexLock(note->entry->lock); /* TODO: is this lock necessary? */
		_note_poll(note);
		mutexUnlock(note->entry->lock);

		note = note->queue_next;
	} while (note != queue->notes);
}


static int queue_unpack(msg_t *msg, evsub_t **subs, int *subcnt, event_t **events, int *evcnt, int *timeout)
{
	if (msg->type == mtRead || msg->type == mtWrite) {
		if (subs != NULL) {
			*subs = msg->i.data;
			*subcnt = msg->i.size / sizeof(evsub_t);
		}

		if (events != NULL) {
			*events = msg->o.data;
			*evcnt = msg->o.size / sizeof(event_t);
		}

		if (timeout != NULL)
			*timeout = (int)msg->i.io.len; /* FIXME: hack! */
	}
#if 0
	else if (msg->type == mtDevCtl) {
		unsigned request;
		event_ioctl_t *ioctl;

		ioctl = (event_ioctl_t *)ioctl_unpack2(msg, &request, NULL, events);
		/* TODO: check request */

		if (subs != NULL) {
			*subs = ioctl->subs;
			*subcnt = ioctl->subcnt;
		}

		if (events != NULL)
			*evcnt = ioctl->eventcnt;

		if (timeout != NULL)
			*timeout = ioctl->timeout;
	}
#endif
	else return -EINVAL;

	return EOK;
}


static void queue_wakeup(evqueue_t *queue)
{
	request_t *r, *filled = NULL, *empty;
	int count = 0;
	event_t *events;
	evqueue_t *q;

	while ((q = queue) != NULL) {
		empty = NULL;

		mutexLock(queue->lock);
		while (queue->requests != NULL) {
			r = queue->requests;
			LIST_REMOVE(&queue->requests, r);

			if (queue_unpack(&r->msg, NULL, NULL, &events, &count, NULL) < 0)
				continue;

			if ((count = _event_read(queue, events, count))) {
				LIST_ADD(&filled, r);
				r->msg.o.io.err = count;
			}
			else {
				LIST_ADD(&empty, r);
			}
		}
		queue->requests = empty;
		mutexUnlock(queue->lock);

		mutexLock(event_common.lock);
		LIST_REMOVE(&queue, queue);
		mutexUnlock(event_common.lock);

		object_put(&q->object);
	}

	while ((r = filled) != NULL) {
		LIST_REMOVE(&filled, r);
		rq_wakeup(r, r->msg.o.io.err);
	}
}


static request_t *queue_close_op(object_t *o, request_t *r)
{
	evqueue_t *queue = evqueue(o);
	request_t *p;
	eventry_t *entry;

	mutexLock(queue->lock);
	while ((p = queue->requests) != NULL) {
		LIST_REMOVE(&queue->requests, p);
		rq_wakeup(p, -EBADF);
	}

	while (queue->notes != NULL) {
		entry_ref(entry = queue->notes->entry);
		mutexLock(entry->lock);
		_note_remove(queue->notes);
		_entry_recalculate(entry);
		mutexUnlock(entry->lock);
		entry_put(entry);
	}
	mutexUnlock(queue->lock);

	object_destroy(o);
	return r;
}


static int _queue_readwrite(evqueue_t *queue, evsub_t *subs, int subcnt, event_t *events, int evcnt)
{
	if (subcnt)
		_event_subscribe(queue, subs, subcnt);

	if (evcnt) {
		_queue_poll(queue);
		evcnt = _event_read(queue, events, evcnt);
	}

	return evcnt;
}


static request_t *queue_write_op(object_t *o, request_t *r)
{
	evqueue_t *queue = evqueue(o);
	int count = 0;

	event_t *events;
	evsub_t *subs;
	int evcnt, subcnt, timeout;

	if (queue_unpack(&r->msg, &subs, &subcnt, &events, &evcnt, &timeout) < 0) {
		r->msg.o.io.err = -EINVAL;
		return r;
	}

	mutexLock(queue->lock);
	if (!(count = _queue_readwrite(queue, subs, subcnt, events, evcnt)) && evcnt && timeout) {
		if (timeout > 0)
			rq_timeout(r, timeout);

		LIST_ADD(&queue->requests, r);
		r = NULL;
	}
	else {
		r->msg.o.io.err = count;
	}
	mutexUnlock(queue->lock);
	return r;
}


static request_t *queue_read_op(object_t *o, request_t *r)
{
	return queue_write_op(o, r);
}


#if 0
static request_t *queue_devctl_op(object_t *o, request_t *r)
{
	evqueue_t *queue = evqueue(o);

	event_t *events = NULL;
	evsub_t *subs = NULL;
	int count, evcnt = 0, subcnt = 0, timeout = 0;

	queue_unpack(&r->msg, &subs, &subcnt, &events, &evcnt, &timeout);

	mutexLock(queue->lock);
	if (!(count = _queue_readwrite(queue, subs, subcnt, events, evcnt))) {
		LIST_ADD(&queue->requests, r);
		rq_timeout(r, timeout);
		r = NULL;
	}
	else {
		rq_setResponse(r, count);
	}
	mutexUnlock(queue->lock);

	return r;
}
#endif


static void queue_timeout_op(request_t *r)
{
	evqueue_t *queue = evqueue(r->object);

	mutexLock(queue->lock);
	LIST_REMOVE(&queue->requests, r);
	mutexUnlock(queue->lock);
}


static request_t *sink_open_op(object_t *o, request_t *r)
{
	return r;
}


static request_t *sink_close_op(object_t *o, request_t *r)
{
	return r;
}


static request_t *sink_create_op(object_t *o, request_t *r)
{
	evqueue_t *queue;

	if ((queue = queue_create()) == NULL) {
		r->msg.o.create.err = -ENOMEM;
	}
	else {
		r->msg.o.create.err = EOK;
		r->msg.o.create.oid.port = srv_port();
		r->msg.o.create.oid.id = object_id(&queue->object);
	}

	return r;
}


static request_t *sink_write_op(object_t *o, request_t *r)
{
	event_t stackbuf[64];
	event_t *events;
	unsigned eventcnt;

	if (r->msg.i.size % sizeof(event_t)) {
		r->msg.o.io.err = -EINVAL;
		return r;
	}

	if (r->msg.i.size <= sizeof(stackbuf)) {
		events = stackbuf;
	}
	else if ((events = malloc(r->msg.i.size)) == NULL) {
		r->msg.o.io.err = -ENOMEM;
		return r;
	}

	eventcnt = r->msg.i.size / sizeof(event_t);
	memcpy(events, r->msg.i.data, r->msg.i.size);
	rq_wakeup(r, EOK);

	event_register(events, eventcnt);

	if (eventcnt > sizeof(stackbuf) / sizeof(event_t))
		free(events);

	return NULL;
}


static request_t *qmx_open_op(object_t *o, request_t *r)
{
	evqueue_t *queue;

	if ((queue = queue_create()) == NULL)
		r->msg.o.io.err = -ENOMEM;
	else
		r->msg.o.io.err = object_id(&queue->object);

	return r;
}


int event_init(void)
{
	if (mutexCreate(&event_common.lock) < 0)
		return -ENOMEM;

	lib_rbInit(&event_common.notes, event_cmp, NULL);
	object_create(&event_common.sink, &sink_ops);
	object_create(&event_common.qmx, &qmx_ops);

	mkdir("/dev/event", 0555);

	object_link(&event_common.sink, "/dev/event/sink");
	object_link(&event_common.qmx, "/dev/event/queue");

	return EOK;
}
