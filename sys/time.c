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


int gettimeofday(struct timeval *restrict tp, void *restrict tzp)
{
	tp->tv_sec = tp->tv_usec = 0;
	return 0;
}
