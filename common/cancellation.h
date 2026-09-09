/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Cancellation point handling
 *
 * Copyright 2026 Phoenix Systems
 * Author: Adam Greloch
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _LIBPHOENIX_WRAP_CANCEL_H_
#define _LIBPHOENIX_WRAP_CANCEL_H_

#include <pthread.h>

#define CANCELLATION_POINT(rettype, function, args) ({ \
	int __oldval = _pthread_enable_asynccancel(); \
	rettype __ret = function args; \
	_pthread_disable_asynccancel(__oldval); \
	__ret; \
})


#define WRAP_ERRNO_DEF_CANCELLATION(rettype, function, arguments, argnames) \
	extern rettype sys_##function arguments; \
	rettype function arguments \
	{ \
		return SET_ERRNO(CANCELLATION_POINT(rettype, sys_##function, argnames)); \
	}


#define WRAP_CANCELLATION(rettype, function, arguments, argnames) \
	extern rettype sys_##function arguments; \
	rettype function arguments \
	{ \
		return CANCELLATION_POINT(rettype, sys_##function, argnames); \
	}

#endif
