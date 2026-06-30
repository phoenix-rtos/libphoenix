/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * assert.h
 *
 * Copyright 2017 Phoenix Systems
 * Author: Pawel Pisarczyk
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_ASSERT_H_
#define _LIBPHOENIX_ASSERT_H_


#include <stdio.h>
#include <stdlib.h>


#ifdef __cplusplus
extern "C" {
#endif

/*
 * We want to be able to include our headers in C++ source and it uses a different
 * keyword for static assertions, the same goes for C from C23 onwards
 */
#if defined(__cplusplus) || __STDC_VERSION__ == 202311L
#define STATIC_ASSERT static_assert
#else
#define STATIC_ASSERT _Static_assert
#endif


#ifndef NDEBUG
#define assert(__expr) \
	((__expr) ? (void)0 : ({ fprintf(stderr, "Assertion '%s' failed in file %s:%d, function %s.\n", #__expr, __FILE__, __LINE__, __func__); abort(); }))
#else


#define assert(expr) ((void) 0)


#endif /* NDEBUG */


#ifdef __cplusplus
}
#endif


#endif /* _LIBPHOENIX_ASSERT_H_ */
