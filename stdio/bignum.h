/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * bignum.h
 *
 * Copyright 2023 Phoenix Systems
 * Author: Andrzej Stalke
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */
#ifndef _LIBPHOENIX_STDIO_BIGNUM_H
#define _LIBPHOENIX_STDIO_BIGNUM_H

#include <stdint.h>
#include <stdlib.h>

#define __BIGNUM_DATA_BITS         (8 * sizeof(__bignum_data_t))
#define __BIGNUM_DATA_MASK         ((__bignum_data_t)(((__bignum_ddata_t)1 << __BIGNUM_DATA_BITS) - 1))
#define DOUBLE_EXP_SHIFT         52
#define DOUBLE_MANTISSA_MASK     ((1LLU << DOUBLE_EXP_SHIFT) - 1)
#define __BIGNUM_STACK_BUFFER_SIZE 4

typedef uint32_t __bignum_data_t;
typedef uint64_t __bignum_ddata_t;

typedef struct {
	size_t len, size;
	__bignum_data_t *data;
	__bignum_data_t stackBuffer[__BIGNUM_STACK_BUFFER_SIZE];
} __bignum_t;

extern int __bignum_init(__bignum_t *bn, size_t size, __bignum_data_t val);


extern void __bignum_destroy(__bignum_t *bn);


extern void __bignum_zero(__bignum_t *bn);


extern int __bignum_shiftl(__bignum_t *bn, size_t shift);


extern void __bignum_shiftr(__bignum_t *bn, size_t shift);


extern int __bignum_cmp(const __bignum_t *first, uint32_t val);


extern int __bignum_copy(__bignum_t *target, const __bignum_t *source);


extern int __bignum_mul(__bignum_t *first, uint32_t val);


extern void __bignum_div(__bignum_t *dividend, uint32_t *remainder, uint32_t divisor);


extern int __bignum_push(__bignum_t *bn, __bignum_data_t val);

#endif /*_LIBPHOENIX_STDIO_BIGNUM_H*/
