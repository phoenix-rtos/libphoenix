/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/times.h
 *
 * Copyright 2018, 2026 Phoenix Systems
 * Author: Jan Sikorski, Adam Greloch
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <errno.h>
#include <time.h>

#include <sys/time.h>
#include <sys/times.h>
#include <sys/types.h>


#define USECS_PER_TICK (1000000 / CLK_TCK)


int sys_cpuTime(pid_t pid, int tid, time_t *cpuTime, cpuTimes_t *cpuTimes);


clock_t times(struct tms *buffer)
{
	cpuTimes_t ct;
	time_t now;
	int err;

	if (buffer == NULL) {
		return (clock_t)SET_ERRNO(-EINVAL);
	}

	err = gettime(&now, NULL);
	if (err < 0) {
		return (clock_t)SET_ERRNO(err);
	}

	err = sys_cpuTime(0, 0, NULL, &ct);
	if (err < 0) {
		return (clock_t)SET_ERRNO(err);
	}

	buffer->tms_utime = (clock_t)(ct.user / USECS_PER_TICK);
	buffer->tms_stime = (clock_t)(ct.sys / USECS_PER_TICK);
	buffer->tms_cutime = (clock_t)(ct.childUser / USECS_PER_TICK);
	buffer->tms_cstime = (clock_t)(ct.childSys / USECS_PER_TICK);

	return (clock_t)(now / USECS_PER_TICK);
}
