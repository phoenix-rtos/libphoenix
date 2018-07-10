/*
 * Phoenix-RTOS
 *
 * posixsrv.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _POSIXSRV_H_
#define _POSIXSRV_H_


enum { pxBufferedPipe, pxPipe };


enum { pxUnlockpt, pxGrantpt };


typedef struct {
	int id;

	union {
		struct {
			int type;
		} pty;
	};
} posixsrv_devctl_t;

#endif
