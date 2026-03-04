/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * ioctl.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */


#ifndef _LIBPHOENIX_IOCTL_H_
#define _LIBPHOENIX_IOCTL_H_


#include <sys/msg.h>
#include <sys/types.h>

#include <phoenix/ioctl.h>


#ifdef __cplusplus
extern "C" {
#endif


/* convenience macro to get sub-pointer `field` in a packed ioctl structure that is read-only */
#define IOC_NESTED_GET_PTR_FIELD(request, subptr, val, field) ioctl_getPointerField(request, subptr, val, offsetof(typeof(*val), field))


#define _IO(g, n)                   _IOC(IOC_VOID, (g), (n), 0)
#define _IOV(g, n, t)               _IOC(IOC_VOID, (g), (n), sizeof(t)) /* IOW with passing by value */
#define _IOR(g, n, t)               _IOC(IOC_OUT, (g), (n), sizeof(t))
#define _IOW(g, n, t)               _IOC(IOC_IN, (g), (n), sizeof(t))
#define _IOWR(g, n, t)              _IOC(IOC_INOUT, (g), (n), sizeof(t))
#define _IOC_NESTED(inout, g, n, t) _IOC((inout) | IOC_NESTED, (g), (n), sizeof(t)) /* passed structure is nested */


#define TIOCGPTN   _IOR('T', 0x30, unsigned int)
#define TIOCSPTLCK _IOW('T', 0x31, int)


#define FIONREAD _IOR('f', 127, int)
#define FIONBIO  _IOW('f', 126, unsigned long)


#define KIOEN _IOV('k', 0, unsigned int)


int ioctl(int fd, unsigned long cmd, ...);


const void *ioctl_unpack(msg_t *msg, unsigned long *request, id_t *id);


const void *ioctl_unpackEx(msg_t *msg, unsigned long *request, id_t *id, void **response_buf);


static inline pid_t ioctl_getSenderPid(const msg_t *msg)
{
	return (pid_t)msg->pid;
}


void ioctl_setResponse(msg_t *msg, unsigned long request, int err, const void *data);


/* get value pointed to by a sub-pointer at offset in nested ioctl structure
   pointed to by val and place it in subptr */
int ioctl_getPointerField(unsigned long request, void **subptr, const void *val, size_t offset);


#ifdef __cplusplus
}
#endif


#endif
