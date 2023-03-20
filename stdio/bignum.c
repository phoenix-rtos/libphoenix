/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * bignum.c
 *
 * Copyright 2023 Phoenix Systems
 * Author: Andrzej Stalke
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include "bignum.h"
#include <errno.h>
#include <string.h>

static void bignum_shiftrInternal(bignum_t *bn, const size_t shift)
{
	bignum_data_t carry = 0, temp;
	const bignum_data_t mask = BIGNUM_DATA_MASK >> (BIGNUM_DATA_BITS - shift);
	ssize_t i;
	for (i = bn->len - 1; i >= 0; --i) {
		temp = bn->data[i];
		if (shift == BIGNUM_DATA_BITS) {
			bn->data[i] = 0;
		}
		else {
			bn->data[i] >>= shift;
		}
		bn->data[i] |= carry;
		carry = (temp & mask) << (BIGNUM_DATA_BITS - shift);
	}
}


static int bignum_resize(bignum_t *bn, const size_t newSize)
{
	bignum_data_t *ptr;
	if (bn->size == newSize) {
		return 0;
	}
	else if (bn->size > newSize) {
		if (bn->data != bn->stackBuffer) {
			if (newSize <= BIGNUM_STACK_BUFFER_SIZE) {
				if (bn->len > newSize) {
					bn->len = newSize;
				}
				(void)memcpy(bn->stackBuffer, bn->data, bn->len * sizeof(*bn->data));
				free(bn->data);
				bn->data = bn->stackBuffer;
				bn->size = newSize;
				return 0;
			}
			else {
				ptr = realloc(bn->data, newSize * sizeof(*bn->data));
				if (ptr != NULL) {
					bn->data = ptr;
					bn->size = newSize;
					if (bn->len > bn->size) {
						bn->len = bn->size;
					}
					return 0;
				}
			}
		}
		else {
			bn->size = newSize;
			if (bn->len > bn->size) {
				bn->len = bn->size;
			}
			return 0;
		}
	}
	else {
		if (newSize <= BIGNUM_STACK_BUFFER_SIZE) {
			bn->size = newSize;
			return 0;
		}
		else if (bn->data == bn->stackBuffer) {
			ptr = calloc(newSize, sizeof(*bn->data));
			if (ptr != NULL) {
				(void)memcpy(ptr, bn->data, bn->size * sizeof(*bn->data));
				bn->size = newSize;
				bn->data = ptr;
				return 0;
			}
		}
		else {
			ptr = realloc(bn->data, sizeof(*bn->data) * newSize);
			if (ptr != NULL) {
				bn->size = newSize;
				bn->data = ptr;
				return 0;
			}
		}
	}
	return -ENOMEM;
}


static int bignum_shiftlInternal(bignum_t *bn, const size_t shift)
{
	bignum_data_t temp, carry = 0;
	const bignum_data_t mask = ~((1LLU << (BIGNUM_DATA_BITS - shift)) - 1);
	size_t i;
	int ret = 0;
	for (i = 0; i < bn->len; ++i) {
		temp = bn->data[i];
		/* Shift by BIGNUM_DATA_BITS is UB, so condition is necessary */
		if (shift == BIGNUM_DATA_BITS) {
			bn->data[i] = 0;
		}
		else {
			bn->data[i] = bn->data[i] << shift;
		}
		bn->data[i] |= carry;
		carry = (temp & mask) >> (BIGNUM_DATA_BITS - shift);
	}
	if (carry != 0) {
		ret = bignum_push(bn, carry);
	}
	return ret;
}


static int bignum_cmpInternal(const bignum_t *first, const bignum_t *second)
{
	size_t i;
	if (first->len < second->len) {
		return -bignum_cmpInternal(second, first);
	}

	for (i = first->len - 1; i >= second->len; --i) {
		if (first->data[i] != 0) {
			return 1;
		}
	}

	i = second->len;
	do {
		--i;
		if (first->data[i] > second->data[i]) {
			return 1;
		}
		else {
			if (first->data[i] < second->data[i]) {
				return -1;
			}
		}
	} while (i != 0);
	return 0;
}


int bignum_init(bignum_t *bn, size_t size, bignum_data_t val)
{
	*bn = (bignum_t) { .data = bn->stackBuffer, .len = 1, .size = 1 };
	bn->data[0] = val;
	return bignum_resize(bn, size);
}

void bignum_destroy(bignum_t *bn)
{
	if (bn != NULL) {
		if (bn->data != bn->stackBuffer) {
			free(bn->data);
		}
		bn->data = NULL;
		bn->len = 0;
		bn->size = 0;
	}
}


void bignum_zero(bignum_t *bn)
{
	size_t i;
	for (i = 0; i < bn->size; ++i) {
		bn->data[i] = 0;
	}
	bn->len = 1;
}


int bignum_shiftl(bignum_t *bn, size_t shift)
{
	int ret;
	while (shift > BIGNUM_DATA_BITS) {
		ret = bignum_shiftlInternal(bn, BIGNUM_DATA_BITS);
		if (ret != 0) {
			return ret;
		}
		shift -= BIGNUM_DATA_BITS;
	}
	return bignum_shiftlInternal(bn, shift);
}


void bignum_shiftr(bignum_t *bn, size_t shift)
{
	while (shift > BIGNUM_DATA_BITS) {
		bignum_shiftrInternal(bn, BIGNUM_DATA_BITS);
		shift -= BIGNUM_DATA_BITS;
	}
	if (shift != 0) {
		bignum_shiftrInternal(bn, shift);
	}
}


/*
 *  1 : first > second
 *  0 : first == second
 * -1 : first < second
 */
int bignum_cmp(const bignum_t *first, uint32_t val)
{
	const bignum_t second = { .data = &val, .len = 1, .size = 1 };
	return bignum_cmpInternal(first, &second);
}


int bignum_copy(bignum_t *target, const bignum_t *source)
{
	size_t i;
	int res = 0;
	if (target->size < source->size) {
		res = bignum_resize(target, source->size);
		if (res != 0) {
			return res;
		}
	}
	for (i = 0; i < source->size; ++i) {
		target->data[i] = source->data[i];
	}
	target->len = source->len;
	return res;
}

int bignum_mul(bignum_t *first, uint32_t val)
{
	size_t i;
	bignum_ddata_t temp;
	bignum_data_t carry = 0;
	int res = 0;

	for (i = 0; i < first->len; ++i) {
		temp = first->data[i];
		temp *= val;
		temp += carry;
		first->data[i] = temp & BIGNUM_DATA_MASK;
		carry = temp >> BIGNUM_DATA_BITS;
	}
	if (carry > 0) {
		res = bignum_push(first, carry);
	}
	return res;
}


void bignum_div(bignum_t *dividend, uint32_t *remainder, uint32_t divisor)
{
	ssize_t i;
	bignum_ddata_t temp = 0;
	for (i = dividend->len - 1; i >= 0; --i) {
		temp <<= BIGNUM_DATA_BITS;
		temp |= dividend->data[i];
		dividend->data[i] = temp / divisor;
		temp %= divisor;
	}
	*remainder = temp;
}


int bignum_push(bignum_t *bn, bignum_data_t val)
{
	int res;
	if (bn->len >= bn->size) {
		res = bignum_resize(bn, bn->size << 1);
		if (res != 0) {
			return res;
		}
	}
	bn->data[bn->len++] = val;
	return 0;
}
