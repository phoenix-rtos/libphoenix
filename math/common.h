/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * math.h common
 *
 * Copyright 2017 Phoenix Systems
 * Author: Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_MATH_COMMON_H_
#define _LIBPHOENIX_MATH_COMMON_H_

#include <stdint.h>


typedef union {
		struct {
#if __BYTE_ORDER == __LITTLE_ENDIAN
			uint64_t mantisa:52;
			uint16_t exponent:11;
			uint8_t sign:1;
#else
			uint8_t sign:1;
			uint16_t exponent:11;
			uint64_t mantisa:52;
#endif
		} i;
		double d;
} conv_t;


extern void normalizeSub(double *x, int *exp);


extern void createSub(double *x, int exp);


extern double quickPow(double x, int e);


extern int isInteger(double x);


#endif
