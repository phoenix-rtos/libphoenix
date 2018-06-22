/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Architecture dependent part (arch/armv7)
 *
 * Copyright 2017 Phoenix Systems
 * Author: Pawel Pisarczyk
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_ARCH_ARMV7_ARCH_H_
#define _LIBPHOENIX_ARCH_ARMV7_ARCH_H_

#include <stddef.h>

#define __BYTE_ORDER __LITTLE_ENDIAN


/* #define NULL (void *)0 - defined in stddef.h */
#define __MEMCPY
#define __MEMCMP
#define __MEMSET
#define __STRLEN
#define __STRNLEN
#define __STRCMP
#define __STRNCMP
#define __STRCPY
#define __STRNCPY
#define __MEMMOVE


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
typedef unsigned long long u64;


typedef signed char s8;
typedef short s16;
typedef int s32;
typedef long long s64;


typedef u32 addr_t;
typedef u32 uintptr_t;


typedef unsigned int size_t;
typedef int ssize_t;
typedef unsigned long long time_t;
typedef unsigned int useconds_t;


typedef u64 offs_t;


typedef u32 id_t;
typedef struct _oid_t {
	u32 port;
	id_t id;
} oid_t;


typedef u32 handle_t;

#include <limits.h>

#define SIZE_PAGE 0x100

#endif
