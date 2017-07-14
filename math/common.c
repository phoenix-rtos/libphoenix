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

#include "common.h"
#include "consts.h"


void normalizeSub(double *x, int *exp)
{
	conv_t *conv = (conv_t *)x;

	if (conv->i.mantisa == 0)
		return;

	while (!(conv->i.mantisa & 0xffffff0000000LL)) {
		conv->i.mantisa <<= 24;
		*exp -= 24;
	}

	while (!(conv->i.mantisa & 0xff00000000000LL)) {
		conv->i.mantisa <<= 8;
		*exp -= 8;
	}

	while (!(conv->i.mantisa & 0xf000000000000LL)) {
		conv->i.mantisa <<= 4;
		*exp -= 4;
	}

	while (!(conv->i.mantisa & 0x8000000000000LL)) {
		conv->i.mantisa <<= 1;
		*exp -= 1;
	}

	/* Subnormals have explicit MSB bit, have to remove it */
	conv->i.mantisa <<= 1;
	*exp -= 1;

	conv->i.exponent = 1;
}


void createSub(double *x, int exp)
{
	conv_t *conv = (conv_t *)&x;

	if (exp < -51) {
		*x = 0.0;
		return;
	}
	else if (exp > 0) {
		return;
	}

	/* Subnormals have explicit MSB bit */
	conv->i.mantisa >>= 1;
	conv->i.mantisa |= (1LL << 52);
	exp += 1;

	if (exp <= -26) {
		conv->i.mantisa >>= 26;
		exp += 26;
	}

	if (exp <= -13) {
		conv->i.mantisa >>= 13;
		exp += 13;
	}

	if (exp <= -8) {
		conv->i.mantisa >>= 8;
		exp += 8;
	}

	while (exp < 0) {
		conv->i.mantisa >>= 1;
		++exp;
	}

	if (conv->i.mantisa == 0)
		*x = 0.0;
}


double quickPow(double x, int e)
{
	double res;
	unsigned int eabs;

	if (e == 0)
		return 1.0;
	else if (e == 1)
		return x;

	res = 1.0;

	if (e < 0) {
		eabs = -e;

		while (eabs != 0) {
			if (eabs & 1)
				res /= x;
			x *= x;
			eabs >>= 1;
		}
	}
	else {
		eabs = e;

		while (eabs != 0) {
			if (eabs & 1)
				res *= x;
			x *= x;
			eabs >>= 1;
		}
	}

	return res;
}


u32 log2(u32 x)
{
	u32 y = 0, b = 1 << 30;
	u64 z = x;
	int i;

	for (i = 0; i < 31; ++i) {
		z = (z * z) >> 31;
		if (z & (2LL << 31)) {
			z >>= 1;
			y |= b;
		}
		b >>= 1;
	}

	return y;
}


int isInteger(double x)
{
	conv_t *conv = (conv_t *)&x;
	int exp = conv->i.exponent - 1023;
	u64 mask = 0xfffffffffffffLL;
	u64 m;

	if (exp > 52)
		return x;
	else if (exp < 0)
		return conv->i.sign? -0.0 : 0.0;

	m = conv->i.mantisa & ~(mask >> exp);

	return m == conv->i.mantisa;
}
