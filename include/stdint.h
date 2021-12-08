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
#endif


#ifdef _USE_STANDARD_TYPES_STDINT
#define INT64_MAX 0x7fffffffffffffffLL
#define INT64_MIN 0x8000000000000000LL
#define UINT64_MAX 0xffffffffffffffffLL

#define INT32_MAX ((int32_t)0x7fffffff)
#define INT32_MIN ((int32_t)0x80000000)
#define UINT32_MAX 0xffffffff

#define INT16_MAX ((int16_t)0x7fff)
#define INT16_MIN ((int16_t)0x8000)
#define UINT16_MAX 0xffff

#define INT8_MAX ((int8_t)0x7f)
#define INT8_MIN ((int8_t)0x80)
#define UINT8_MAX 0xff
#endif


#endif
