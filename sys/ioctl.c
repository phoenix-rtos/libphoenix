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

#include <sys/ioctl.h>
#include <phoenix/ioctl.h>
#include <sys/minmax.h>
#include <string.h>

/* SIOCGIFCONF handling */
#include <sys/socket.h>
#include <sys/sockios.h>
#include <net/if.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>


struct ioctl_data {
	void 	*indata;
	void 	*outdata;
	size_t 	insz;
	size_t 	outsz;
	int retval;
};

extern int fileIoctl(int fd, unsigned long req, void *indata, size_t insz, void *outdata, size_t outsz);


static int ioctl_serialize(unsigned long request, struct ioctl_data *ioc, void *indata)
{
	void *iocdata = indata;
	size_t size = IOCPARM_LEN(request);

	if ((request & IOC_INOUT) && !iocdata)
		return -EINVAL;

	/* Special cases, WARNING: all special ioctls are assumed to be defined as IOCWR */
	if (request == SIOCGIFCONF) {
		/* Unpack and send the buffer directly */
		struct ifconf *ifc = iocdata;
		iocdata = ifc->ifc_req;
		size = ifc->ifc_len;
	}
	else if (request == SIOCADDRT || request == SIOCDELRT) {
		/* arg is structure with pointer - has to be custom-packed into message */
		struct rtentry *rt = (struct rtentry *)iocdata;
		size_t dev_len;
		if (rt->rt_dev != NULL) {
			dev_len = strlen(rt->rt_dev) + 1;
			iocdata = calloc(1, size + dev_len);
			if (!iocdata)
				return -ENOMEM;
			memcpy(iocdata, indata, size);
			memcpy(iocdata + size, rt->rt_dev, dev_len);
			size += dev_len;
		}
	}

	if (request & IOC_IN) {
		ioc->indata = iocdata;
		ioc->insz = size;
	}

	if ((request & IOC_OUT)) {
		ioc->outdata = iocdata;
		ioc->outsz = size;
	}
	return 0;
}

static void ioctl_deserialize(unsigned long request, struct ioctl_data *ioc, void *outdata)
{
	if (request == SIOCGIFCONF) {
		struct ifreq *ifr = (struct ifconf *)ioc->outdata;
		size_t len = ioc->retval;
		struct ifconf *oifc = (struct ifconf *)outdata;
		oifc->ifc_len = len;

		if (oifc->ifc_buf != NULL) {
			memcpy(oifc->ifc_buf, ioc->outdata, len);
		}

		ioc->retval = 0;
	}

//	if (ioc->outdata != outdata)
//		free(ioc->outdata);
}

static int do_ioctl(int fd, unsigned long request, void *data)
{
	int ret;
	struct ioctl_data ioc = { 0 };

	ret = ioctl_serialize(request, &ioc, data);

	if (ret)
		return ret;

	ioc.retval = fileIoctl(fd, request, ioc.indata, ioc.insz, ioc.outdata, ioc.outsz);

	ioctl_deserialize(request, &ioc, data);

	return ioc.retval;
}

void ioctl_setResponse(msg_t *msg, unsigned long request, int err, const void *data)
{
	size_t size = IOCPARM_LEN(request);

	msg->o.io = err;
	if ((request & IOC_OUT) && data != NULL)
		memcpy(msg->o.data, data, size);
}

void ioctl_setResponseErr(msg_t *msg, unsigned long request, int err)
{
	msg->o.io = err;
}


int ioctl(int fildes, unsigned long request, ...)
{
	va_list ap;
	void *val = NULL;

	if (request & IOC_INOUT) {
		va_start(ap, request);
		val = va_arg(ap, void *);
		va_end(ap);
	}

	return SET_ERRNO(do_ioctl(fildes, request, val));
}