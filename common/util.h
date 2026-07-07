/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Common libphoenix utilities
 *
 * Copyright 2026 Phoenix Systems
 * Author: Julian Uziembło
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_COMMON_UTIL_H_
#define _LIBPHOENIX_COMMON_UTIL_H_

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>


/* 0 if str is NULL, else length + 1 to account for terminator byte */
static inline size_t __strSizeNull(const char *str)
{
	return (str == NULL) ? 0 : (strlen(str) + 1);
}


static inline bool __timevalValid(const struct timeval *tv)
{
	return (tv != NULL) && (tv->tv_usec >= 0) && (tv->tv_usec < 1000 * 1000);
}


static inline bool __timespecValid(const struct timespec *ts)
{
	return (ts != NULL) && (ts->tv_nsec >= 0) && (ts->tv_nsec < 1000L * 1000L * 1000L);
}


#endif /* _LIBPHOENIX_COMMON_UTIL_H_ */
