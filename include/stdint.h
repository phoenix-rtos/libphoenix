/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * stdint.h
 *
 * Copyright 2017-2019, 2024 Phoenix Systems
 * Author: Pawel Pisarczyk, Andrzej Glowinski, Lukasz Leczkowski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_STDINT_H_
#define _LIBPHOENIX_STDINT_H_


#include <arch.h>


#ifdef __ARCH_STDINT
#include __ARCH_STDINT
#else
#error "Required header stdint.h is not defined for current architectue!"
#endif

#ifdef _USE_STANDARD_TYPES_STDINT

/* The following sections refer to ISO/IEC 9899:1999 */

/* 7.18.1.1 Exact-width integer types */

typedef __s8 int8_t;
typedef __s16 int16_t;
typedef __s32 int32_t;
typedef __s64 int64_t;

typedef __u8 uint8_t;
typedef __u16 uint16_t;
typedef __u32 uint32_t;
typedef __u64 uint64_t;

/* 7.18.1.2 Minimum-width integer types */

typedef int8_t int_least8_t;
typedef int16_t int_least16_t;
typedef int32_t int_least32_t;
typedef int64_t int_least64_t;

typedef uint8_t uint_least8_t;
typedef uint16_t uint_least16_t;
typedef uint32_t uint_least32_t;
typedef uint64_t uint_least64_t;

/* 7.18.2.1 Limits of exact-width integer types */

#define INT8_MIN  (-128)
#define INT16_MIN (-32768)
#define INT32_MIN (-2147483648)
#define INT64_MIN (-INT64_C(9223372036854775808))

#define INT8_MAX  (127)
#define INT16_MAX (32767)
#define INT32_MAX (2147483647)
#define INT64_MAX (INT64_C(9223372036854775807))

#define UINT8_MAX  (0xffU)
#define UINT16_MAX (0xffffU)
#define UINT32_MAX (0xffffffffU)
#define UINT64_MAX (UINT64_C(0xffffffffffffffff))

/* 7.18.2.2 Limits of minimum-width integer types */

#define INT_LEAST8_MIN  INT8_MIN
#define INT_LEAST16_MIN INT16_MIN
#define INT_LEAST32_MIN INT32_MIN
#define INT_LEAST64_MIN INT64_MIN

#define INT_LEAST8_MAX  INT8_MAX
#define INT_LEAST16_MAX INT16_MAX
#define INT_LEAST32_MAX INT32_MAX
#define INT_LEAST64_MAX INT64_MAX

#define UINT_LEAST8_MAX  UINT8_MAX
#define UINT_LEAST16_MAX UINT16_MAX
#define UINT_LEAST32_MAX UINT32_MAX
#define UINT_LEAST64_MAX UINT64_MAX

#endif /* _USE_STANDARD_TYPES_STDINT */


#endif /* _LIBPHOENIX_STDINT_H_ */
