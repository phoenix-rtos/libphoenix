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

	if (conv->i.exponent != 0)
		return;
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


/* WARNING: Assumes IEEE 754 double-precision binary floating-point format */
double frexp(double x, int* exp)
{
	conv_t *conv = (conv_t *)&x;

	*exp = 0;

	if (x == 0.0)
		return 0.0;

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

	normalizeSub(&x, &exponent);

	exponent += conv->i.exponent + exp;

	if (exponent > 2046) {
		/* TODO: errno ERANGE */
		return conv->i.sign ? -HUGE_VAL : HUGE_VAL;
	}

	/* If result is subnormal */
	if (exponent < 0) {
		if (exponent < -51)
			return 0.0;

		/* Subnormals have explicit MSB bit */
		conv->i.mantisa >>= 1;
		conv->i.mantisa |= (1LL << 52);
		exponent += 1;

		if (exponent <= -26) {
			conv->i.mantisa >>= 26;
			exponent += 26;
		}

		if (exponent <= -13) {
			conv->i.mantisa >>= 13;
			exponent += 13;
		}

		if (exponent <= -8) {
			conv->i.mantisa >>= 8;
			exponent += 8;
		}

		while (exponent < 0) {
			conv->i.mantisa >>= 1;
			++exponent;
		}

		if (conv->i.mantisa == 0)
			conv->i.sign = 0;
	}
	else {
		conv->i.exponent = exponent;
	}

	return x;
}


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

#if 0
double log(double x)
{
	return 0;
}


double log10(double x)
{
	return 0;
}


double modf(double x, double* intpart)
{
	return 0;
}
#endif
