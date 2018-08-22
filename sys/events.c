/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * interface to posixsrv's events
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <sys/msg.h>
#include <sys/events.h>
#include <errno.h>


static struct {
	int sink_open;
	oid_t sink;
} event_common;


int eventsSend(event_t *event, int count)
{
	msg_t msg;
	int err = -ENOSYS;

	if (event_common.sink_open || (event_common.sink_open = (lookup("/dev/event/sink", NULL, &event_common.sink) == EOK))) {
		msg.type = mtWrite;

		msg.i.io.oid = event_common.sink;
		msg.i.data = event;
		msg.i.size = count * sizeof(event_t);
		msg.o.data = NULL;
		msg.o.size = 0;

		err = msgSend(event_common.sink.port, &msg);
	}

	return err;
}
