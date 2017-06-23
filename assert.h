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


#ifdef NDEBUG

#define assert(expr) do { \
		printf("Assertion failed in file %s:%d, function %s.\n", __FILE__, __LINE__, __func__); \
		exit(1); \
	} while (0)

#else


#define assert(expr)


#endif



#endif
