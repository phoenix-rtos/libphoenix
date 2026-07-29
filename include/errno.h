/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Error definitions
 *
 * Copyright 2017, 2018 Phoenix Systems
 * Author: Pawel Pisarczyk
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_ERRNO_H_
#define _LIBPHOENIX_ERRNO_H_


#include <phoenix/errno.h>


#ifdef __cplusplus
extern "C" {
#endif


#define errno (*__errno_location())


extern int *__errno_location(void);


static inline int set_errno(int x)
{
	if (x < 0) {
		errno = -x;
		return -1;
	}

	return x;
}


#define SET_ERRNO set_errno


#define WRAP_ERRNO_DEF(rettype, function, arguments, argnames) \
	extern rettype sys_##function arguments; \
	rettype function arguments \
	{ \
		return SET_ERRNO(sys_##function argnames); \
	}


#ifdef __cplusplus
}
#endif


#endif
