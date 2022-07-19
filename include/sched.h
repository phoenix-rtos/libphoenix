/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sched
 *
 * Copyright 2019 Phoenix Systems
 * Author: Marcin Baran
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_SCHED_H_
#define _LIBPHOENIX_SCHED_H_


#include <errno.h>


#ifdef __cplusplus
extern "C" {
#endif


#define SCHED_FIFO 0
#define SCHED_RR 1
#define SCHED_OTHER 2


struct sched_param {
	int sched_priority;
};


extern int sched_yield(void);


extern int sched_get_priority_max(int policy);


extern int sched_get_priority_min(int policy);


#ifdef __cplusplus
}
#endif


#endif
