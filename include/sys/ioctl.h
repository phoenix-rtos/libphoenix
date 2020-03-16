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


#define TIOCGPTN	_IOR('T', 0x30, unsigned int)
#define TIOCSPTLCK	_IOW('T', 0x31, int)


int ioctl(int fd, unsigned long cmd, ... );

void ioctl_setResponse(msg_t *msg, unsigned long request, int err, const void *data);

void ioctl_setResponseErr(msg_t *msg, unsigned long request, int err);


#endif
