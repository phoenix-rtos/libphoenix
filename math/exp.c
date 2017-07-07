/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * exp, frexp, ldexp, log, log10, modf
 *
 * Copyright 2017 Phoenix Systems
 * Author: Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include ARCH
#include "consts.h"


typedef union {
		struct {
#ifdef __LITTLE_ENDIAN
			u64 mantisa:52;
			u16 exponent:11;
			u8 sign:1;
#else
			u8 sign:1;
			u16 exponent:11;
			u64 mantisa:52;
#endif
		} i;
		double d;
} conv_t;


static void normalizeSub(double *x, int *exp)
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


static void createSub(double *x, int exp)
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


/* Uses Clay S. Turner's Fast Binary Logarithm Algorithm */
static u32 log2(u32 x)
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


/* WARNING: Assumes IEEE 754 double-precision binary floating-point format */
double frexp(double x, int* exp)
{
	conv_t *conv = (conv_t *)&x;

	*exp = 0;

	if (x == 0.0)
		return 0.0;

	if (conv->i.exponent == 0)
		normalizeSub(&x, exp);

	*exp += conv->i.exponent - 1022;
	conv->i.exponent = 1022;

	return x;
}


/* WARNING: Assumes IEEE 754 double-precision binary floating-point format */
double ldexp(double x, int exp)
{
	conv_t *conv = (conv_t *)&x;
	int exponent = 0;

	if (x == 0.0)
		return 0.0;

	if (conv->i.exponent == 0)
		normalizeSub(&x, &exponent);

	exponent += conv->i.exponent + exp;

	if (exponent > 2046) {
		/* TODO: errno ERANGE */
		return conv->i.sign ? -HUGE_VAL : HUGE_VAL;
	}

	/* If result is subnormal */
	if (exponent < 0) {
		createSub(&x, exponent);
		conv->i.exponent = 0;
	}
	else {
		conv->i.exponent = exponent;
	}

	return x;
}


/* Uses Maclaurin series to calculate value of e^x */
double exp(double x) /* TODO - not tested */
{
	double res, powx;
	int strong, i;

	strong = 1;
	res = 1;
	powx = x;

	for (i = 0; i < 11; ++i) { /* TODO pick number of iterations */
		res += powx / strong;
		strong *= strong + 1;
		powx *= x;
	}

	return res;
}


/* Uses ln(x) = log2(x) / log2(e) and
 * log2(x) = log2(M * 2^E) = log2(M) + E identities
 * WARNING: Assumes IEEE 754 double-precision binary floating-point format */
double log(double x)
{
	conv_t *conv = (conv_t *)&x;
	int exp = 0;
	u32 tmp;

	if (x < 0) {
		/* TODO errno EDOM */
		return NAN;
	}
	else if (x == 0) {
		/* TODO errno ERANGE */
		return -HUGE_VAL;
	}
	else if (x == 1) {
		return 0.0;
	}

	exp = conv->i.exponent - 1023;

	if (conv->i.exponent == 0)
		normalizeSub(&x, &exp);

	tmp = conv->i.mantisa >> 21;
	if (conv->i.mantisa & 0x1fffff)
		++tmp;
	tmp |= (1 << 31);
	tmp = log2(tmp);
	conv->i.mantisa = (u64)tmp << 21;

	normalizeSub(&x, &exp);

	if (1023 + exp <= 0) {
		createSub(&x, exp + conv->i.exponent);
		conv->i.exponent = 0;
	}
	else {
		conv->i.exponent = 1023;
	}

	return (x + exp - 1) / M_LOG2E;
}


/* Uses log10(x) = ln(x) / ln(10) identity */
double log10(double x)
{
	return log(x) / M_LN10;
}

#if 0
double modf(double x, double* intpart)
{
	return 0;
}
#endif
