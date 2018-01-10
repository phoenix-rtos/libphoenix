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


int gettimeofday(struct timeval *restrict tp, void *restrict tzp);

#endif
