/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/ioctl.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_SYS_IOCTL_H_
#define _LIBPHOENIX_SYS_IOCTL_H_

#include <sys/msg.h>
#include <sys/types.h>
#include <sys/sockios.h>
#include <phoenix/ioctl.h>


#define FIONREAD   _IOR('f', 0x7f, int)           /* Get number of bytes to read */
#define FIONBIO    _IOW('f', 0x7e, unsigned long) /* Set/clear non-blocking I/O */
#define TIOCGPTN   _IOR('T', 0x30, unsigned int)  /* Number of pty */
#define TIOCSPTLCK _IOW('T', 0x31, int)           /* Lock/unlock pty slave device */


extern int ioctl(int fd, unsigned long cmd, ... );


extern const void *ioctl_unpack(const msg_t *msg, unsigned long *request, id_t *id);


extern const void *ioctl_unpackEx(const msg_t *msg, unsigned long *request, id_t *id, void** response_buf);


extern pid_t ioctl_getSenderPid(const msg_t *msg);


extern void ioctl_setResponse(msg_t *msg, unsigned long request, int err, const void *data);


extern void ioctl_setResponseErr(msg_t *msg, unsigned long request, int err);


#endif
