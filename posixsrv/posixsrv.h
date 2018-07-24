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


enum { pxUnlockpt, pxGrantpt, pxPtsname };


typedef struct {
	int id;
	int type;

	union {
		struct {
		} unlockpt;
		struct {
		} grantpt;
		struct {
			char *buf;
			size_t len;
		} ptsname;
	};
} posixsrv_devctl_t;

#endif
