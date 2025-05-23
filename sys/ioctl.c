/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/ioctl.c
 *
 * Copyright 2018, 2023 Phoenix Systems
 * Author: Krystian Wasik, Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <arch.h>

#include <sys/ioctl.h>
#include <phoenix/ioctl.h>
#include <sys/minmax.h>
#include <string.h>

#include <sys/socket.h>


const void *ioctl_unpack(msg_t *msg, unsigned long *request, id_t *id)
{
	return ioctl_unpackEx(msg, request, id, NULL);
}


const void *ioctl_unpackEx(msg_t *msg, unsigned long *request, id_t *id, void **response_buf)
{
	size_t size;
	const void *data = NULL;
	ioctl_in_t *ioctl = (ioctl_in_t *)msg->i.raw;

	if (request != NULL) {
		*request = ioctl->request;
	}

	if (id != NULL) {
		*id = msg->oid.id;
	}

	size = ioctl->size;

	if ((ioctl->request & IOC_IN) != 0) {
		if (size <= (sizeof(msg->i.raw) - sizeof(ioctl_in_t))) {
			data = ioctl->data;
		}
		else {
			data = msg->i.data;
		}
	}
	else if (((ioctl->request & IOC_INOUT) == 0) && (size > 0)) {
		/* the data is passed by value instead of pointer */
		size = min(size, sizeof(void *));
		(void)memcpy(&data, ioctl->data, size);
	}
	else {
	}

	if ((response_buf != NULL) && ((ioctl->request & IOC_OUT) != 0)) {
		if (size <= sizeof(msg->o.raw)) {
			*response_buf = msg->o.raw;
		}
		else {
			*response_buf = msg->o.data;
		}

		if ((ioctl->request & IOC_IN) != 0) {
			(void)memcpy(*response_buf, data, size);
		}
	}

	return data;
}


void ioctl_setResponse(msg_t *msg, unsigned long request, int err, const void *data)
{
	size_t size = IOCPARM_LEN(request);
	void *dst;

	msg->o.err = err;

	if (((request & IOC_OUT) != 0) && (data != NULL)) {
		if (size <= sizeof(msg->o.raw)) {
			dst = msg->o.raw;
		}
		else {
			dst = msg->o.data;
		}

		(void)memcpy(dst, data, size);
	}
}
