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

#include <posixsrv.h>


enum { evtDataOut = 0, evtDataIn, evtError, evtGone };


extern int eventsSend(event_t *event, int count);

#endif
