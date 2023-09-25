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

#define BIGNUM_DATA_BITS         (8 * sizeof(bignum_data_t))
#define BIGNUM_DATA_MASK         ((bignum_data_t)(((bignum_ddata_t)1 << BIGNUM_DATA_BITS) - 1))
#define DOUBLE_EXP_SHIFT         52
#define DOUBLE_MANTISSA_MASK     ((1LLU << DOUBLE_EXP_SHIFT) - 1)
#define BIGNUM_STACK_BUFFER_SIZE 4

typedef uint32_t bignum_data_t;
typedef uint64_t bignum_ddata_t;

typedef struct bignum {
	size_t len, size;
	bignum_data_t *data;
	bignum_data_t stackBuffer[BIGNUM_STACK_BUFFER_SIZE];
} bignum_t;

extern int bignum_init(bignum_t *bn, size_t size, bignum_data_t val);


extern void bignum_destroy(bignum_t *bn);


extern void bignum_zero(bignum_t *bn);


extern int bignum_shiftl(bignum_t *bn, size_t shift);


extern void bignum_shiftr(bignum_t *bn, size_t shift);


extern int bignum_cmp(const bignum_t *first, uint32_t val);


extern int bignum_copy(bignum_t *target, const bignum_t *source);


extern int bignum_mul(bignum_t *first, uint32_t val);


extern void bignum_div(bignum_t *dividend, uint32_t *remainder, uint32_t divisor);


extern int bignum_push(bignum_t *bn, bignum_data_t val);

#endif /*_LIBPHOENIX_STDIO_BIGNUM_H*/
