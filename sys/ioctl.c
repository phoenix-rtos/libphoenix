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


const void * ioctl_unpack(const msg_t *msg, unsigned long *request)
{
	unsigned long req;
	size_t size;
	const void *data = NULL;

	memcpy(&req, msg->i.raw, sizeof(unsigned long));

	if (request != NULL)
		*request = req;

	size = IOCPARM_LEN(req);

	if (req & IOC_IN) {
		if (size <= (sizeof(msg->i.raw) - sizeof(unsigned long))) {
			data = (void *)msg->i.raw + sizeof(unsigned long);
		} else {
			data = msg->i.data;
		}
	}

	return data;
}


void ioctl_setResponse(msg_t *msg, unsigned long request, int err, const void *data)
{
	size_t size = IOCPARM_LEN(request);
	void *dst;

	memcpy(msg->o.raw, &err, sizeof(unsigned long));

	if ((request & IOC_OUT) && data != NULL) {
		if (size <= (sizeof(msg->o.raw) - sizeof(unsigned long))) {
			dst = (void *)msg->o.raw + sizeof(unsigned long);
		} else {
			dst = msg->o.data;
		}

		memcpy(dst, data, size);
	}
}
