/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/time.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _SYS_TIME_H_
#define _SYS_TIME_H_

#include ARCH


struct timeval {
	time_t tv_sec;
	time_t tv_usec;
};


extern int gettimeofday(struct timeval *tp, void *tzp);


extern int utimes(const char *filename, const struct timeval times[2]);

#endif
