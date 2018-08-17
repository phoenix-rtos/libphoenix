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


enum { evAdd = 0x1, evDelete = 0x2, evEnable = 0x4, evDisable = 0x8, evOneshot = 0x10, evClear = 0x20, evDispatch = 0x40 };


typedef struct {
	oid_t oid;
	unsigned flags;
	unsigned short types;
} evsub_t;


typedef struct _event_t {
	oid_t oid;
	unsigned type;

	unsigned flags;
	unsigned count;
	unsigned data;
} event_t;


typedef struct _event_ioctl_t {
	int eventcnt;
	int subcnt;
	int timeout;
	evsub_t subs[];
} event_ioctl_t;


#endif
