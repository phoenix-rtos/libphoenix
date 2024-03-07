/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * interface to posixsrv's events
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */


#ifndef _SYS_EVENTS_H_
#define _SYS_EVENTS_H_

#include <sys/msg.h>
#include <phoenix/events.h>


#ifdef __cplusplus
extern "C" {
#endif


enum { evtDataOut = 0, evtDataIn, evtError, evtGone };


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


extern int eventsSend(event_t *event, int count);


#ifdef __cplusplus
}
#endif


#endif /* _SYS_EVENTS_H_ */
