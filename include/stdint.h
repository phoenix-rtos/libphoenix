/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * stdint.h
 *
 * Copyright 2017 Phoenix Systems
 * Author: Pawel Pisarczyk
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_STDINT_H_
#define _LIBPHOENIX_STDINT_H_

#include <arch.h>


typedef u8 uint8_t;
typedef u16 uint16_t;
typedef u32 uint32_t;
typedef u64 uint64_t;

typedef s8 int8_t;
typedef s16 int16_t;
typedef s32 int32_t;
typedef s64 int64_t;

typedef uint64_t uintmax_t;
typedef int64_t intmax_t;

#ifndef SIZE_MAX
#define SIZE_MAX (~(size_t)0)
#endif

#ifndef INT64_MAX
#define INT64_MAX 0x7fffffffffffffffll
#endif

#ifndef INT64_MIN
#define INT64_MIN 0x8000000000000000ll
#endif

#ifndef UINT64_MAX
#define UINT64_MAX 0xffffffffffffffffll
#endif

#ifndef UINT64_MIN
#define UINT64_MIN 0x0000000000000000ll
#endif

#ifndef INT32_MAX
#define INT32_MAX ((int32_t)0x7fffffff)
#endif

#ifndef INT32_MIN
#define INT32_MIN ((int32_t)0x80000000)
#endif

#ifndef UINT32_MAX
#define UINT32_MAX 0xffffffff
#endif

#ifndef INT16_MAX
#define INT16_MAX ((int16_t)0x7fff)
#endif

#ifndef INT16_MIN
#define INT16_MIN ((int16_t)0x8000)
#endif

#ifndef UINT16_MAX
#define UINT16_MAX 0xffff
#endif

#ifndef INT8_MAX
#define INT8_MAX ((int8_t)0x7f)
#endif

#ifndef INT8_MIN
#define INT8_MIN ((int8_t)0x80)
#endif

#ifndef UINT8_MAX
#define UINT8_MAX 0xff
#endif

#endif
