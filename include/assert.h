/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * assert.h
 *
 * Copyright 2017, 2026 Phoenix Systems
 * Author: Pawel Pisarczyk, Michal Lach, Ziemowit Leszczynski
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _LIBPHOENIX_ASSERT_H_
#define _LIBPHOENIX_ASSERT_H_


#include <stdio.h>
#include <stdlib.h>


#ifdef __cplusplus
extern "C" {
#endif

/*
 * `static_assert` is a keyword in C++ since C++11 and in C since C23. C11 through
 * C17 spell it `_Static_assert` and require <assert.h> to provide the macro below,
 * which is what makes the name usable from both languages.
 */
#if !defined(__cplusplus) && (!defined(__STDC_VERSION__) || __STDC_VERSION__ < 202311L)
#ifndef static_assert
#define static_assert _Static_assert
#endif
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
