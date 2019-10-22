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


struct ioctl_data {
	void 	*indata;
	void 	*outdata;
	size_t 	insz;
	size_t 	outsz;
};

extern int sys_ioctl(int fildes, unsigned long request, void *val);


static void *ioctl_serialize(, unsigned long req, struct ioctl_data *ioc, void *indata)
{
	void *iocdata = indata;

	if ((request & IOC_INOUT) && !iocdata)
		return -EINVAL;

	/* ioctl special case: arg is structure with pointer - has to be custom-packed into message
	 * WARNING: all special ioctls are assumed to be defined as IOCWR */
	if (request == SIOCGIFCONF) {
		struct ifconf *ifc = (struct ifconf*)iocdata;
		iocdata = calloc(1, size + ifc->ifc_len);
		if (!iocdata)
			return -ENOMEM;
		memcpy(iocdata, data, size);
		memcpy(iocdata + size, ifc->ifc_buf, ifc->ifc_len);
		size += ifc->ifc_len;
	}
	else if (request == SIOCADDRT || request == SIOCDELRT) {
		struct rtentry *rt = (struct rtentry *)iocdata;
		size_t dev_len;
		if (rt->rt_dev != NULL) {
			dev_len = strlen(rt->rt_dev) + 1;
			iocdata = calloc(1, size + dev_len);
			if (!iocdata)
				return -ENOMEM;
			memcpy(iocdata, data, size);
			memcpy(iocdata + size, dev_len);
			size += dev_len;
		}
	}

	if (request & IOC_IN) {
		ioc->indata = iocdata;
		ioc->insz = size;
	}

	if ((request & IOC_OUT)) {
		ioc->outdata = iocdata;
		ioc->outsz = size
	}
}

static void *ioctl_deserialize(unsigned long req, struct ioctl_data *ioc, void *outdata)
{
	if (request == SIOCGIFCONF) {
		struct ifconf *ifc = (struct ifconf*)ioc->outdata;
		struct ifconf *oifc = (struct ifconf*)outdata;
		memcpy(oifc->ifc_buf, data + sizeof(struct ifconf), ifc->ifc_len);
		oifc->ifc_len = ifc->ifc_len;
	}

	if (ioc.outdata != outdata)
		free(iocdata);

	return out_data;
}

static int do_ioctl(int fd, unsigned long request, void *data)
{
	int ret;
	struct ioctl_data ioc = { 0 };
	size_t size = IOCPARM_LEN(request);

	ioctl_serialize(request, &ioc, data);

	ret = sys_ioctl(fd, request, ioc.indata, ioc.insz, ioc.outdata, ioc.outsz);

	ioctl_deserialize(requset, &ioc, data);

	return ret;
}

const void * ioctl_getData(const msg_t *msg, unsigned long *request, id_t *id)
{
	*id = msg.id;

	if (request == SIOCGIFCONF) {
		struct ifconf *ifc = (struct ifconf*)msg.o.data;
		ifc->ifc_len = ((struct ifconf*)msg.i.data)->ifc_len;
		ifc->ifc_buf = msg.o.data + (struct ifconf);
		return ifc;
	}
	else if (request == SIOCADDRT || request == SIOCDELRT) {
		struct rtentry *rt = (struct rtentry *)msg.o.data;
		memcpy(msg.o.data, msg.i.data, msg.i.size);
		rt->rt_dev = msg.o.data + sizeof(struct rtentry);
		return rt;
	}

	if (request & IOC_IN)
		return msg.i.data;

	return msg.o.data;
}

pid_t ioctl_getSenderPid(const msg_t *msg)
{
	return msg->pid;
}

void ioctl_setResponse(msg_t *msg, unsigned long request, int err, const void *data)
{
	size_t size = IOCPARM_LEN(request);

	if ((request & IOC_OUT) && data != NULL)
		memcpy(&msg->o.data, data, size);
}

void ioctl_setResponseErr(msg_t *msg, unsigned long request, int err)
{
	/* TBD */
}


int ioctl(int fildes, unsigned long request, ...)
{
	va_list ap;
	void *val = NULL;
	struct ioctl_struct ioc;

	if (request & IOC_INOUT) {
		va_start(ap, request);
		val = va_arg(ap, void *);
		va_end(ap);
	}

	return SET_ERRNO(do_ioctl(fildes, request, val));
}