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


#ifndef NDEBUG
#define assert(__expr) \
    ((__expr) \
     ? (void)0 \
     : ({ printf("Assertion '%s' failed in file %s:%d, function %s.\n", #__expr, __FILE__, __LINE__, __func__); exit(1);}))
#else


#define assert(expr)


#endif


#endif
