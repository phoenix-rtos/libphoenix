/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Architecture dependent part of stdint (arch/ia32)
 *
 * Copyright 2019-2021, 2024 Phoenix Systems
 * Author: Andrzej Glowinski, Daniel Sawka, Lukasz Leczkowski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_ARCH_IA32_STDINT_H_
#define _LIBPHOENIX_ARCH_IA32_STDINT_H_

#include <phoenix/arch/ia32/stdtypes.h>

/* The following sections refer to ISO/IEC 9899:1999 */

/* 7.18.1.3 Fastest minimum-width integer types */

typedef __u8 int_fast8_t;
typedef __s32 int_fast16_t;
typedef __s32 int_fast32_t;
typedef __s64 int_fast64_t;

typedef __u8 uint_fast8_t;
typedef __u32 uint_fast16_t;
typedef __u32 uint_fast32_t;
typedef __u64 uint_fast64_t;


/* Define other fixed types along with defines/macros */

#define _USE_STANDARD_TYPES_STDINT

/* 7.18.1.4 Integer types capable of holding object pointers */

typedef __s32 intptr_t;
typedef __u32 uintptr_t;

/* 7.18.1.5 Greatest-width integer types */

typedef __s64 intmax_t;
typedef __u64 uintmax_t;

/* 7.18.2.3 Limits of fastest minimum-width integer types */

#define INT_FAST8_MIN  INT8_MIN
#define INT_FAST16_MIN INT32_MIN
#define INT_FAST32_MIN INT32_MIN
#define INT_FAST64_MIN INT64_MIN

#define INT_FAST8_MAX  INT8_MAX
#define INT_FAST16_MAX INT32_MAX
#define INT_FAST32_MAX INT32_MAX
#define INT_FAST64_MAX INT64_MAX

#define UINT_FAST8_MAX  UINT8_MAX
#define UINT_FAST16_MAX UINT32_MAX
#define UINT_FAST32_MAX UINT32_MAX
#define UINT_FAST64_MAX UINT64_MAX

/* 7.18.2.4 Limits of integer types capable of holding object pointers */

#define INTPTR_MIN  INT32_MIN
#define INTPTR_MAX  INT32_MAX
#define UINTPTR_MAX UINT32_MAX

/* 7.18.2.5 Limits of greatest-width integer types */

#define INTMAX_MIN  INT64_MIN
#define INTMAX_MAX  INT64_MAX
#define UINTMAX_MAX UINT64_MAX

/* 7.18.3 Limits of other integer types */

#define PTRDIFF_MIN INT32_MIN
#define PTRDIFF_MAX INT32_MAX

#define SIZE_MAX UINT32_MAX

/* 7.18.4.1 Macros for minimum-width integer constants */

#define INT8_C(c)  c
#define INT16_C(c) c
#define INT32_C(c) c
#define INT64_C(c) c##LL

#define UINT8_C(c)  c##U
#define UINT16_C(c) c##U
#define UINT32_C(c) c##U
#define UINT64_C(c) c##ULL

/* 7.18.4.2 Macros for greatest-width integer constants */

#define INTMAX_C(c)  c##LL
#define UINTMAX_C(c) c##ULL


/* Additional definitions */

#define _PRI_8   ""
#define _PRI_16  ""
#define _PRI_32  ""
#define _PRI_64  "ll"
#define _SCN_8   "hh"
#define _SCN_16  "h"
#define _SCN_32  ""
#define _SCN_64  "ll"
#define _PRI_PTR ""

#define _SCN_LEAST8  _SCN_8
#define _SCN_LEAST16 _SCN_16
#define _SCN_LEAST32 _SCN_32
#define _SCN_LEAST64 _SCN_64
#define _SCN_FAST8   _SCN_8
#define _SCN_FAST16  _SCN_32
#define _SCN_FAST32  _SCN_32
#define _SCN_FAST64  _SCN_64

#endif
