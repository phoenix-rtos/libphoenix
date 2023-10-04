/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * types.h
 *
 * Copyright 2018, 2019 Phoenix Systems
 * Author: Jan Sikorski, Marcin Baran
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

#ifdef __ARCH_SYS_TYPES
#include __ARCH_SYS_TYPES
#else
#error "Required header sys/types.h is not defined for current architectue!"
#endif

typedef int pid_t;
typedef int mode_t;
typedef int gid_t;
typedef int uid_t;

typedef int dev_t;
typedef int ino_t;
typedef int nlink_t;
typedef int blksize_t;
typedef long long blkcnt_t;
typedef unsigned long long fsblkcnt_t;
typedef unsigned long long fsfilcnt_t;
typedef long long off64_t;
typedef off64_t off_t;

typedef int clock_t;
typedef int clockid_t;

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


typedef struct pthread_mutexattr_t {
	int type;
} pthread_mutexattr_t;


typedef struct {
	handle_t condh;
	volatile int initialized;
	clockid_t clock_id;
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
