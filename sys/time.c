/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/time.c
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <sys/time.h>
#include <errno.h>
#include <stdlib.h>

#include "posix/utils.h"


int utimes(const char *filename, const struct timeval times[2])
{
	return SET_ERRNO(-ENOSYS);
}


int gettimeofday(struct timeval *tp, void *tzp)
{
	time_t t, o;

	/* TODO: Set errno if failed */
	gettime(&t, &o);

	tp->tv_sec = (t + o) / (1000 * 1000);
	tp->tv_usec = (t + o) % (1000 * 1000);

	return 0;
}


int stime(const time_t *t)
{
	struct timeval tv;

	tv.tv_sec = *t;
	tv.tv_usec = 0;

	return settimeofday(&tv, NULL);
}


int settimeofday(const struct timeval *tv, void *tz)
{
	time_t t;

	/* TODO: Set errno if failed */
	gettime(&t, NULL);

	/* TODO: Set errno if failed */
	settime(tv->tv_usec + tv->tv_sec * 1000 * 1000 - t);

	return 0;
}


int futimes(int fd, const struct timeval tv[2])
{
	return 0;
}


int lutimes(const char *filename, const struct timeval tv[2])
{
	return 0;
}

int timerisset(struct timeval *tvp)
{
	return 0;
}
