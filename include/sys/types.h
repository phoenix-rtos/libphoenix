/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * types.h
 *
 * Copyright 2018, 2019, 2024 Phoenix Systems
 * Author: Jan Sikorski, Marcin Baran, Lukasz Leczkowski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _SYS_TYPES_H
#define _SYS_TYPES_H

#include <arch.h>
#include <stddef.h>
#include <stdint.h>

#include <phoenix/types.h>


typedef int clock_t;
typedef int clockid_t;

typedef unsigned int useconds_t;
typedef int suseconds_t;

typedef char *caddr_t;

typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned int u_int;
typedef unsigned long u_long;

typedef struct pthread_attr_t {
	void *stackaddr;
	int policy;
	int priority;
	int detached;
	size_t stacksize;
} pthread_attr_t;

typedef uintptr_t pthread_t;

typedef struct {
	handle_t mutexh;
	volatile int initialized;
} pthread_mutex_t;

/* TODO */
typedef int pthread_rwlock_t;

typedef int pthread_rwlockattr_t;


typedef struct lockAttr pthread_mutexattr_t;


typedef struct {
	handle_t condh;
	volatile int initialized;
} pthread_cond_t;


typedef struct pthread_condattr_t {
	int pshared;
	clockid_t clock_id;
} pthread_condattr_t;

typedef struct __pthread_key_t *pthread_key_t;

typedef uint32_t pthread_once_t;

/* BSD legacy types permitted by POSIX */
typedef uint8_t   u_int8_t;
typedef uint16_t  u_int16_t;
typedef uint32_t  u_int32_t;
typedef uint64_t  u_int64_t;
typedef int register_t;

#endif
