/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * utime.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_UTIME_H_
#define _LIBPHOENIX_UTIME_H_


#include <sys/types.h>


#ifdef __cplusplus
extern "C" {
#endif


struct utimbuf {
	time_t actime;
	time_t modtime;
};


int utime(const char *path, const struct utimbuf *times);


#ifdef __cplusplus
}
#endif


#endif
