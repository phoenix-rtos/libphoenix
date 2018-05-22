/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * time
 *
 * Copyright 2017 Phoenix Systems
 * Author: Andrzej Asztemborski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include ARCH

#include "time.h"
#include "errno.h"


time_t time(time_t *tp) {
	time_t now;

	gettime(&now);

	/* microseconds to seconds */
	now += 500 * 1000;
	now /= 1000 * 1000;

	if (tp != NULL)
		*tp = now;

	return now;
}


int clock_gettime(clockid_t clk_id, struct timespec *tp)
{
	/* TODO - make use of clk_id */

	time_t now;

	if (tp == NULL)
		return -EINVAL;

	gettime(&now);

	tp->tv_sec = now / (1000 * 1000);
	now -= tp->tv_sec * 1000 * 1000;
	tp->tv_nsec = now * 1000;

	return EOK;
}


struct tm *localtime(const time_t *timer)
{
	return NULL;
}
