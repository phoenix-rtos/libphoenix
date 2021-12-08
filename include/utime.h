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


struct utimbuf {
	time_t actime;  /* Access time */
	time_t modtime; /* Modification time */
};


int utime(const char *path, const struct utimbuf *times);


#endif
