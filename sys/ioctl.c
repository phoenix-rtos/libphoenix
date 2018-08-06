/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/ioctl.c
 *
 * Copyright 2018 Phoenix Systems
 * Author: Krystian Wasik
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <arch.h>

#include <phoenix/ioctl.h>
#include <sys/ioctl.h>
#include <string.h>


const void * ioctl_unpack(const msg_t *msg, unsigned long *request, id_t *id)
{
	size_t size;
	const void *data = NULL;
	ioctl_in_t *ioctl = (ioctl_in_t *)msg->i.raw;

	if (request != NULL)
		*request = ioctl->request;

	if (id != NULL)
		*id = ioctl->id;

	size = IOCPARM_LEN(ioctl->request);

	if (ioctl->request & IOC_IN) {
		if (size <= (sizeof(msg->i.raw) - sizeof(ioctl_in_t)))
			data = ioctl->data;
		else
			data = msg->i.data;
	}

	return data;
}


void ioctl_setResponse(msg_t *msg, unsigned long request, int err, const void *data)
{
	size_t size = IOCPARM_LEN(request);
	void *dst;
	ioctl_out_t *ioctl = (ioctl_out_t *)msg->o.raw;

	ioctl->err = err;

	if ((request & IOC_OUT) && data != NULL) {
		if (size <= (sizeof(msg->o.raw) - sizeof(ioctl_out_t)))
			dst = ioctl->data;
		else
			dst = msg->o.data;

		memcpy(dst, data, size);
	}
}
