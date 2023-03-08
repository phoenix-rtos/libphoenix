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

/* SIOCGIFCONF handling */
#include <sys/socket.h>
#include <sys/sockios.h>
#include <net/if.h>
#include <stdlib.h>


const void *ioctl_unpack(const msg_t *msg, unsigned long *request, id_t *id)
{
	return ioctl_unpackEx(msg, request, id, NULL);
}


const void *ioctl_unpackEx(const msg_t *msg, unsigned long *request, id_t *id, void **response_buf)
{
	size_t size;
	const void *data = NULL;
	ioctl_in_t *ioctl = (ioctl_in_t *)msg->i.raw;

	if (request != NULL) {
		*request = ioctl->request;
	}

	if (id != NULL) {
		*id = ioctl->id;
	}

	size = IOCPARM_LEN(ioctl->request);

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

	/* ioctl special case: arg is structure with pointer - has to be custom-packed into message */
	if (ioctl->request == SIOCGIFCONF) {
		struct ifconf *ifc = (struct ifconf *)data;
		if (ifc->ifc_len > 0) {
			ifc->ifc_buf = msg->o.data;
		}
	}
	else if ((ioctl->request == SIOCADDRT) || (ioctl->request == SIOCDELRT)) {
		/* input data is read only so we have allocate the in_data if
		 * we want to change it. TODO: find better place to allocate and free
		 * message */
		struct rtentry *rt = malloc(msg->i.size);
		if (rt == NULL) {
			return NULL;
		}
		(void)memcpy(rt, msg->i.data, msg->i.size);
		rt->rt_dev = malloc(msg->o.size);
		if (rt->rt_dev == NULL) {
			free(rt);
			return NULL;
		}
		(void)memcpy(rt->rt_dev, msg->o.data, msg->o.size);
		data = (void *)rt;
	}
	else {
	}

	if ((response_buf != NULL) && ((ioctl->request & IOC_OUT) != 0)) {
		ioctl_out_t *ioctl_out = (ioctl_out_t *)msg->o.raw;
		if (size <= (sizeof(msg->o.raw) - sizeof(ioctl_out_t))) {
			*response_buf = ioctl_out->data;
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


pid_t ioctl_getSenderPid(const msg_t *msg)
{
	const ioctl_in_t *ioctl = (const ioctl_in_t *)msg->i.raw;
	return (pid_t)ioctl->pid;
}


void ioctl_setResponse(msg_t *msg, unsigned long request, int err, const void *data)
{
	size_t size = IOCPARM_LEN(request);
	void *dst;
	ioctl_out_t *ioctl = (ioctl_out_t *)msg->o.raw;

	ioctl->err = err;

	if (((request & IOC_OUT) != 0) && (data != NULL)) {
		if (size <= (sizeof(msg->o.raw) - sizeof(ioctl_out_t))) {
			dst = ioctl->data;
		}
		else {
			dst = msg->o.data;
		}

		(void)memcpy(dst, data, size);
	}
}


void ioctl_setResponseErr(msg_t *msg, unsigned long request, int err)
{
	ioctl_out_t *ioctl = (ioctl_out_t *)msg->o.raw;

	(void)request;

	ioctl->err = err;
}
