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


static inline size_t __strlenNull(const char *str)
{
	return (str == NULL) ? 0 : strlen(str);
}


#endif /* _LIBPHOENIX_COMMON_UTIL_H_ */
