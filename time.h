/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * stdlib.h
 *
 * Copyright 2017 Phoenix Systems
 * Author: Pawel Pisarczyk
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_TIME_H_
#define _LIBPHOENIX_TIME_H_

#include ARCH

#define SECS_TO_USECS_T(secs) (1000000ULL * (secs))
#define MSECS_TO_USECS_T(msecs) (1000ULL * (msecs))

#include <sys/types.h>


typedef enum { CLOCK_MONOTONIC = 0, CLOCK_REALTIME } clockid_t;


struct tm {
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
	int tm_yday;
	int tm_isdst;
};


struct timespec {
	time_t tv_sec;
	long tv_nsec;
};


extern int gettime(time_t *tp);


extern int clock_gettime(clockid_t clk_id, struct timespec *tp);


extern time_t time(time_t *tp);


#endif
