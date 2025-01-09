/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Architecture dependent part (arch/aarch64)
 *
 * Copyright 2017, 2018 Phoenix Systems
 * Author: Pawel Pisarczyk, Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_ARCH_AARCH64_ARCH_H_
#define _LIBPHOENIX_ARCH_AARCH64_ARCH_H_

#define __ARCH_STDINT <arch/aarch64/stdint.h>
#define __ARCH_LIMITS <arch/aarch64/limits.h>

#define __MEMCPY
#define __MEMSET
#define __MEMMOVE

#ifndef __SOFTFP__
/* clang-format off */
#define __IEEE754_SQRT
#define __ieee754_sqrt(x) ({ double a = (x); __asm__ ("fsqrt %d0, %d1" : "=w"(a) : "w"(a)); a; })

#define __IEEE754_SQRTF
#define __ieee754_sqrtf(x) ({ float a = (x); __asm__ ("fsqrt %s0, %s1" : "=w"(a) : "w"(a)); a; })

#define __IEEE754_FABS
#define __ieee754_fabs(x) ({ double a = (x); __asm__ ("fabs %d0, %d1" : "=w"(a) : "w"(a)); a; })

#define __IEEE754_FABSF
#define __ieee754_fabsf(x) ({ float a = (x); __asm__ ("fabs %s0, %s1" : "=w"(a) : "w"(a)); a; })

#define __IEEE754_CEIL
#define __ieee754_ceil(x) ({ double a = (x); __asm__ ("frintp %d0, %d1" : "=w"(a) : "w"(a)); a; })

#define __IEEE754_CEILF
#define __ieee754_ceilf(x) ({ float a = (x); __asm__ ("frintp %s0, %s1" : "=w"(a) : "w"(a)); a; })

#define __IEEE754_FLOOR
#define __ieee754_floor(x) ({ double a = (x); __asm__ ("frintm %d0, %d1" : "=w"(a) : "w"(a)); a; })

#define __IEEE754_FLOORF
#define __ieee754_floorf(x) ({ float a = (x); __asm__ ("frintm %s0, %s1" : "=w"(a) : "w"(a)); a; })

#define __IEEE754_ROUND
#define __ieee754_round(x) ({ double a = (x); __asm__ ("frinta %d0, %d1" : "=w"(a) : "w"(a)); a; })

#define __IEEE754_ROUNDF
#define __ieee754_roundf(x) ({ float a = (x); __asm__ ("frinta %s0, %s1" : "=w"(a) : "w"(a)); a; })

#define __IEEE754_TRUNC
#define __ieee754_trunc(x) ({ double a = (x); __asm__ ("frintz %d0, %d1" : "=w"(a) : "w"(a)); a; })

#define __IEEE754_TRUNCF
#define __ieee754_truncf(x) ({ float a = (x); __asm__ ("frintz %s0, %s1" : "=w"(a) : "w"(a)); a; })
/* clang-format on */
#endif

#define _PAGE_SIZE 0x1000
#define SIZE_PAGE  _Pragma("GCC warning \"'SIZE_PAGE' is deprecated. Use _PAGE_SIZE from arch.h or PAGE_SIZE from limits.h (POSIX only)\"") _PAGE_SIZE

#define __LIBPHOENIX_ARCH_TLS_SUPPORTED

#endif
