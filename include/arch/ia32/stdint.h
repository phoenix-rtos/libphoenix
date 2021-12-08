/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Architecture dependent integer types
 *
 * Copyright 2019 Phoenix Systems
 * Author: Andrzej Glowinski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_ARCH_IA32_STDINT_H_
#define _LIBPHOENIX_ARCH_IA32_STDINT_H_


typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

typedef signed char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef long long int64_t;

/* Define other fixed types along with defines/macros */

#define _USE_STANDARD_TYPES_STDINT

#define SIZE_MAX UINT32_MAX

typedef uint64_t uintmax_t;
typedef int64_t intmax_t;

#define INTMAX_MAX INT64_MAX
#define INTMAX_MIN INT64_MIN
#define UINTMAX_MAX UINT64_MAX

typedef uint32_t uintptr_t;
typedef int32_t intptr_t;

#define INTPTR_MIN INT32_MIN
#define INTPTR_MAX INT32_MAX
#define UINTPTR_MAX UINT32_MAX

#define _PRI_8 ""
#define _PRI_16 ""
#define _PRI_32 ""
#define _PRI_64 "ll"


#endif
