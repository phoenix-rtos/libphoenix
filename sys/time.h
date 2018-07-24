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

#include <arch.h>


struct timeval {
	time_t tv_sec;
	time_t tv_usec;
};


struct timezone {
	int tz_minuteswest;
	int tz_dsttime;
};


extern int gettime(time_t *raw, time_t *offs);


extern int settime(time_t offs);


extern int gettimeofday(struct timeval *tp, void *tzp);


extern int stime(const time_t *t);


/* For compability with Linux software */
extern int settimeofday(const struct timeval *tv, void *tz);


extern int utimes(const char *filename, const struct timeval times[2]);


extern int futimes(int fd, const struct timeval tv[2]);


extern int lutimes(const char *filename, const struct timeval tv[2]);

#endif
