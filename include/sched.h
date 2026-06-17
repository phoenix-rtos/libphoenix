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
#include <phoenix/sched.h>


#ifdef __cplusplus
extern "C" {
#endif


struct sched_param {
	int sched_priority;
};


int schedInfo(pid_t pid, int policy, sched_info_t *info);


int sched_yield(void);


int sched_get_priority_max(int policy);


int sched_get_priority_min(int policy);


#ifdef __cplusplus
}
#endif


#endif
