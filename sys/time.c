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


int gettimeofday(struct timeval *tp, void *tzp)
{
	tp->tv_sec = tp->tv_usec = 0;
	return 0;
}


int utimes(const char *filename, const struct timeval times[2])
{
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
