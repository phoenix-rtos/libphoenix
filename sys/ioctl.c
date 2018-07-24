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

#include <sys/ioctl.h>
#include <string.h>


const void * ioctl_unpack(const msg_t *msg, int *request)
{
	int req;
	size_t size;
	const void *data = NULL;

	memcpy(&req, msg->i.raw, sizeof(int));

	if (request != NULL)
		*request = req;

	size = IOCPARM_LEN(req);

	if (req & IOC_IN) {
		if (size <= (sizeof(msg->i.raw) - sizeof(int))) {
			data = (void *)msg->i.raw + sizeof(int);
		} else {
			data = msg->i.data;
		}
	}

	return data;
}


void ioctl_setResponse(msg_t *msg, int request, int err, const void *data)
{
	size_t size = IOCPARM_LEN(request);
	void *dst;

	memcpy(msg->o.raw, &err, sizeof(int));

	if ((request & IOC_OUT) && data != NULL) {
		if (size <= (sizeof(msg->o.raw) - sizeof(int))) {
			dst = (void *)msg->o.raw + sizeof(int);
		} else {
			dst = msg->o.data;
		}

		memcpy(dst, data, size);
	}
}
