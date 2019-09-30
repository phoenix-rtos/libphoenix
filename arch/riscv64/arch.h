/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Architecture dependent part (RISCV64)
 *
 * Copyright 2018 Phoenix Systems
 * Author: Pawel Pisarczyk
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_ARCH_RISCV64_ARCH_H_
#define _LIBPHOENIX_ARCH_RISCV64_ARCH_H_

#include <stddef.h>

#define __BYTE_ORDER __LITTLE_ENDIAN
#define HAVE_EFFICIENT_UNALIGNED_ACCESS 1

/* #define NULL (void *)0 - defined in stddef */

#define __MEMCPY
#define __MEMSET


#define max(a, b) ({ \
  __typeof__ (a) _a = (a); \
  __typeof__ (b) _b = (b); \
  _a > _b ? _a : _b; })


#define min(a, b) ({ \
  __typeof__ (a) _a = (a); \
  __typeof__ (b) _b = (b); \
        _a > _b ? _b : _a; \
})


typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long u64;

typedef signed char s8;
typedef short s16;
typedef int s32;
typedef long s64;

typedef u64 addr_t;
typedef u64 cycles_t;
typedef u64 uintptr_t;

typedef u64 usec_t;
typedef s64 offs_t;

typedef int ssize_t;
typedef u64 time_t;
typedef unsigned int useconds_t;


typedef u64 id_t;
typedef struct _oid_t {
	u32 port;
	id_t id;
} oid_t;


typedef u32 handle_t;


#define SIZE_PAGE 0x1000


#endif
