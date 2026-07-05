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
#include <phoenix/posix-types.h>
#include <phoenix/posix-timespec.h>


#ifdef __cplusplus
extern "C" {
#endif


struct sched_param {
	int sched_priority;
};


int sched_yield(void);


int sched_get_priority_max(int policy);


int sched_get_priority_min(int policy);


int sched_setparam(pid_t pid, const struct sched_param *param);


int sched_getparam(pid_t pid, struct sched_param *param);


int sched_setscheduler(pid_t pid, int policy, const struct sched_param *param);


int sched_getscheduler(pid_t pid);


int sched_rr_get_interval(pid_t pid, struct timespec *tp);


#ifdef __cplusplus
}
#endif


#endif
