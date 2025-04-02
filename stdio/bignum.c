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

static void __bignum_shiftrInternal(__bignum_t *bn, const size_t shift)
{
	__bignum_data_t carry = 0, temp;
	const __bignum_data_t mask = __BIGNUM_DATA_MASK >> (__BIGNUM_DATA_BITS - shift);
	ssize_t i;
	for (i = bn->len - 1; i >= 0; --i) {
		temp = bn->data[i];
		if (shift == __BIGNUM_DATA_BITS) {
			bn->data[i] = 0;
		}
		else {
			bn->data[i] >>= shift;
		}
		bn->data[i] |= carry;
		carry = (temp & mask) << (__BIGNUM_DATA_BITS - shift);
	}
}


static int __bignum_resize(__bignum_t *bn, const size_t newSize)
{
	__bignum_data_t *ptr;
	if (bn->size == newSize) {
		return 0;
	}
	else if (bn->size > newSize) {
		if (bn->data != bn->stackBuffer) {
			if (newSize <= __BIGNUM_STACK_BUFFER_SIZE) {
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
		if (newSize <= __BIGNUM_STACK_BUFFER_SIZE) {
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


static int __bignum_shiftlInternal(__bignum_t *bn, const size_t shift)
{
	__bignum_data_t temp, carry = 0;
	const __bignum_data_t mask = ~((1LLU << (__BIGNUM_DATA_BITS - shift)) - 1);
	size_t i;
	int ret = 0;
	for (i = 0; i < bn->len; ++i) {
		temp = bn->data[i];
		/* Shift by __BIGNUM_DATA_BITS is UB, so condition is necessary */
		if (shift == __BIGNUM_DATA_BITS) {
			bn->data[i] = 0;
		}
		else {
			bn->data[i] = bn->data[i] << shift;
		}
		bn->data[i] |= carry;
		carry = (temp & mask) >> (__BIGNUM_DATA_BITS - shift);
	}
	if (carry != 0) {
		ret = __bignum_push(bn, carry);
	}
	return ret;
}


static int __bignum_cmpInternal(const __bignum_t *first, const __bignum_t *second)
{
	size_t i;
	if (first->len < second->len) {
		return -__bignum_cmpInternal(second, first);
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


int __bignum_init(__bignum_t *bn, size_t size, __bignum_data_t val)
{
	*bn = (__bignum_t) { .data = bn->stackBuffer, .len = 1, .size = 1 };
	bn->data[0] = val;
	return __bignum_resize(bn, size);
}

void __bignum_destroy(__bignum_t *bn)
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


void __bignum_zero(__bignum_t *bn)
{
	size_t i;
	for (i = 0; i < bn->size; ++i) {
		bn->data[i] = 0;
	}
	bn->len = 1;
}


int __bignum_shiftl(__bignum_t *bn, size_t shift)
{
	int ret;
	while (shift > __BIGNUM_DATA_BITS) {
		ret = __bignum_shiftlInternal(bn, __BIGNUM_DATA_BITS);
		if (ret != 0) {
			return ret;
		}
		shift -= __BIGNUM_DATA_BITS;
	}
	return __bignum_shiftlInternal(bn, shift);
}


void __bignum_shiftr(__bignum_t *bn, size_t shift)
{
	while (shift > __BIGNUM_DATA_BITS) {
		__bignum_shiftrInternal(bn, __BIGNUM_DATA_BITS);
		shift -= __BIGNUM_DATA_BITS;
	}
	if (shift != 0) {
		__bignum_shiftrInternal(bn, shift);
	}
}


/*
 *  1 : first > second
 *  0 : first == second
 * -1 : first < second
 */
int __bignum_cmp(const __bignum_t *first, uint32_t val)
{
	const __bignum_t second = { .data = &val, .len = 1, .size = 1 };
	return __bignum_cmpInternal(first, &second);
}


int __bignum_copy(__bignum_t *target, const __bignum_t *source)
{
	size_t i;
	int res = 0;
	if (target->size < source->size) {
		res = __bignum_resize(target, source->size);
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

int __bignum_mul(__bignum_t *first, uint32_t val)
{
	size_t i;
	__bignum_ddata_t temp;
	__bignum_data_t carry = 0;
	int res = 0;

	for (i = 0; i < first->len; ++i) {
		temp = first->data[i];
		temp *= val;
		temp += carry;
		first->data[i] = temp & __BIGNUM_DATA_MASK;
		carry = temp >> __BIGNUM_DATA_BITS;
	}
	if (carry > 0) {
		res = __bignum_push(first, carry);
	}
	return res;
}


void __bignum_div(__bignum_t *dividend, uint32_t *remainder, uint32_t divisor)
{
	ssize_t i;
	__bignum_ddata_t temp = 0;
	for (i = dividend->len - 1; i >= 0; --i) {
		temp <<= __BIGNUM_DATA_BITS;
		temp |= dividend->data[i];
		dividend->data[i] = temp / divisor;
		temp %= divisor;
	}
	*remainder = temp;
}


int __bignum_push(__bignum_t *bn, __bignum_data_t val)
{
	int res;
	if (bn->len >= bn->size) {
		res = __bignum_resize(bn, bn->size << 1);
		if (res != 0) {
			return res;
		}
	}
	bn->data[bn->len++] = val;
	return 0;
}
