/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * exp, frexp, ldexp, log, log10
 *
 * Copyright 2017 Phoenix Systems
 * Author: Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <errno.h>
#include <math.h>
#include <stdint.h>
#include "common.h"


double frexp(double x, int* exp)
{
	conv_t *conv = (conv_t *)&x;

	*exp = 0;

	if (x == 0.0 || x == -0.0)
		return x;

	if (conv->i.exponent == 0)
		normalizeSub(&x, exp);

	*exp += conv->i.exponent - 1022;
	conv->i.exponent = 1022;

	return x;
}


double ldexp(double x, int exp)
{
	conv_t *conv = (conv_t *)&x;
	int exponent = 0;

	if (x == 0.0 || x == -0.0)
		return x;

	if (conv->i.exponent == 0)
		normalizeSub(&x, &exponent);

	exponent += conv->i.exponent + exp;

	if (exponent > 2046) {
		errno = ERANGE;
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


double log(double x)
{
	double tmp, pow, res;
	conv_t *conv = (conv_t *)&tmp;
	int exp = 0, i;

	if (x < 0) {
		errno = EDOM;
		return NAN;
	}
	else if (x == 0) {
		errno = ERANGE;
		return -HUGE_VAL;
	}
	else if (x == 1.0) {
		return 0.0;
	}

	tmp = x;

	exp = conv->i.exponent - 1022;

	if (conv->i.exponent == 0)
		normalizeSub(&tmp, &exp);

	conv->i.exponent = 1022;

	tmp = (tmp - 1.0) / (tmp + 1.0);

	for (i = 1, res = 0, pow = tmp * tmp; i < 16; ++i) {
		res += tmp / (2 * i - 1);
		tmp *= pow;
	}

	return (2 * res) + (exp / M_LOG2E);
}


double log2(double x)
{
	return log(x) / M_LN2;
}


/* Uses log10(x) = ln(x) / ln(10) identity */
double log10(double x)
{
	return log(x) / M_LN10;
}


/* Uses quick powering and Maclaurin series to calculate value of e^x */
double exp(double x)
{
	double res, resi, powx, e, factorial;
	int i;

	if (x > 710.0)
		return HUGE_VAL;

	/* Get floor of exponent */
	x = modf(x, &e);

	/* Calculate most of the result */
	resi = quickPow(M_E, (int)e);

	/* Calculate rest of the result using Maclaurin series */
	factorial = 1.0;
	powx = x;
	res = 1.0;

	for (i = 2; i < 13; ++i) {
		if (powx == 0.0 || powx == -0.0)
			break;
		res += powx / factorial;
		factorial *= i;
		powx *= x;
	}

	return res * resi;
}
