/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * stdint.h
 *
 * Copyright 2017-2019 Phoenix Systems
 * Author: Pawel Pisarczyk, Andrzej Glowinski
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

/* 7.18.2.1 Limits of exact-width integer types */

#define INT8_MIN  ((int8_t)0x80)
#define INT16_MIN ((int16_t)0x8000)
#define INT32_MIN ((int32_t)0x80000000)
#define INT64_MIN ((int64_t)0x8000000000000000LL)

#define INT8_MAX  ((int8_t)0x7f)
#define INT16_MAX ((int16_t)0x7fff)
#define INT32_MAX ((int32_t)0x7fffffff)
#define INT64_MAX ((int64_t)0x7fffffffffffffffLL)

#define UINT8_MAX  0xff
#define UINT16_MAX 0xffff
#define UINT32_MAX 0xffffffff
#define UINT64_MAX 0xffffffffffffffffLL

#endif /* _USE_STANDARD_TYPES_STDINT */


#endif /* _LIBPHOENIX_STDINT_H_ */
