/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/types.h
 *
 * Copyright 2018, 2019, 2021 Phoenix Systems
 * Author: Jan Sikorski, Marcin Baran, Lukasz Kosinski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_SYS_TYPES_H_
#define _LIBPHOENIX_SYS_TYPES_H_

#include <stdint.h>
#include <phoenix/posix/types.h>


/* POSIX thread types */
typedef uintptr_t pthread_t;
typedef handle_t pthread_mutex_t;
typedef void *pthread_mutexattr_t;

typedef struct {
	void *stackaddr;
	int policy;
	int priority;
	int detached;
	size_t stacksize;
} pthread_attr_t;


/* BSD legacy types permitted by POSIX */
typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned int u_int;
typedef unsigned long u_long;

typedef uint8_t u_int8_t;
typedef uint16_t u_int16_t;
typedef uint32_t u_int32_t;
typedef uint64_t u_int64_t;

typedef int register_t;
typedef char * caddr_t;


#endif
