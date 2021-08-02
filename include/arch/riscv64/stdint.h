/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Architecture dependent part of stdint (arch/ia32)
 *
 * Copyright 2019 Phoenix Systems
 * Author: Andrzej Glowinski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_ARCH_RISCV64_STDINT_H_
#define _LIBPHOENIX_ARCH_RISCV64_STDINT_H_

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long uint64_t;

typedef uint8_t uint_fast8_t;
typedef uint16_t uint_fast16_t;
typedef uint32_t uint_fast32_t;
typedef uint64_t uint_fast64_t;

typedef uint8_t uint_least8_t;

typedef signed char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef long int64_t;

typedef int8_t int_fast8_t;
typedef int16_t int_fast16_t;
typedef int32_t int_fast32_t;
typedef int64_t int_fast64_t;

#define UINT64_C(c) c##UL

/* Define other fixed types along with defines/macros */

#define _USE_STANDARD_TYPES_STDINT

#define SIZE_MAX UINT64_MAX

typedef uint64_t uintmax_t;
typedef int64_t intmax_t;

#define INTMAX_MAX  INT64_MAX
#define INTMAX_MIN  INT64_MIN
#define UINTMAX_MAX UINT64_MAX

typedef uint64_t uintptr_t;
typedef int64_t intptr_t;

#define INTPTR_MIN  INT64_MIN
#define INTPTR_MAX  INT64_MAX
#define UINTPTR_MAX UINT64_MAX

#define _PRI_8  ""
#define _PRI_16 ""
#define _PRI_32 ""
#define _PRI_64 "l"

#endif
