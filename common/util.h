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

#include <stdlib.h>
#include <string.h>


/* 0 if str is NULL, else length + 1 to account for terminator byte */
static inline size_t __strSizeNull(const char *str)
{
	return (str == NULL) ? 0 : (strlen(str) + 1);
}


#endif /* _LIBPHOENIX_COMMON_UTIL_H_ */
