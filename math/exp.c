/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * exp, frexp, ldexp, log, log10, modf, ceil, floor, fmod, fabs
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


double frexp(double x, int *exp)
{
	if (isnan(x) != 0) {
		return NAN;
	}

	if (isinf(x) != 0) {
		return x;
	}

	if (x == 0.0) {
		return x;
	}

	conv_t *conv = (conv_t *)&x;
	*exp = 0;

	if (conv->i.exponent == 0) {
		normalizeSub(&x, exp);
	}

	*exp += conv->i.exponent - 1022;
	conv->i.exponent = 1022;

	return x;
}


double ldexp(double x, int exp)
{
	if (isnan(x) != 0) {
		return NAN;
	}

	if (x == 0.0) {
		return x;
	}

	conv_t *conv = (conv_t *)&x;
	int exponent = 0;

	if (conv->i.exponent == 0) {
		normalizeSub(&x, &exponent);
	}

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

	if (isnan(x) != 0) {
		return NAN;
	}
	else if (x < 0.0) {
		errno = EDOM;
		return NAN;
	}
	else if (x == 0.0) {
		errno = ERANGE;
		return -HUGE_VAL;
	}
	else if (x == 1.0) {
		return 0.0;
	}
	else if (isinf(x) != 0) {
		return x;
	}

	tmp = x;

	exp = conv->i.exponent - 1022;

	if (conv->i.exponent == 0) {
		normalizeSub(&tmp, &exp);
	}

	conv->i.exponent = 1022;

	tmp = (tmp - 1.0) / (tmp + 1.0);

	for (i = 1, res = 0.0, pow = tmp * tmp; i < 16; ++i) {
		res += tmp / ((2 * i) - 1);
		tmp *= pow;
	}

	return ((2.0 * res) + (exp / M_LOG2E));
}


double log2(double x)
{
	return (log(x) / M_LN2);
}


/* Uses log10(x) = ln(x) / ln(10) identity */
double log10(double x)
{
	return (log(x) / M_LN10);
}


double modf(double x, double *intpart)
{
	conv_t *conv = (conv_t *)&x;
	double tmp = x;
	int exp = conv->i.exponent - 1023;
	uint64_t m, mask = 0xfffffffffffffLL;

	if (isnan(x) != 0) {
		*intpart = NAN;
		return NAN;
	}

	if (exp > 52) {
		*intpart = x;
		return (conv->i.sign ? -0.0 : 0.0);
	}
	else if (exp < 0) {
		*intpart = conv->i.sign ? -0.0 : 0.0;
		return x;
	}

	conv->i.mantisa = conv->i.mantisa & ~(mask >> exp);
	*intpart = x;
	x = tmp;

	m = conv->i.mantisa;
	m &= mask >> exp;

	if (m == 0u) {
		return 0.0;
	}

	conv->i.mantisa = m & mask;
	normalizeSub(&x, &exp);

	conv->i.exponent = exp + 1023;

	return x;
}

float modff(float x, float *intpart)
{
	double ret, tmp;

	ret = modf(x, &tmp);
	*intpart = tmp;

	return ret;
}

/* Uses quick powering and Maclaurin series to calculate value of e^x */
double exp(double x)
{
	double res, resi, powx, e, factorial;
	int i;

	if (isnan(x) != 0) {
		return NAN;
	}

	/* Values of x greater than 709.79 will cause overflow, returning INFINITY */
	if (x > 709.79) {
		errno = ERANGE;
		return HUGE_VAL;
	}

	/* Get floor of exponent */
	x = modf(x, &e);

	/* Calculate most of the result */
	resi = quickPow(M_E, (int)e);

	/* Calculate rest of the result using Maclaurin series */
	factorial = 1.0;
	powx = x;
	res = 1.0;

	for (i = 2; i < 13; ++i) {
		if (powx == 0.0) {
			break;
		}
		res += powx / factorial;
		factorial *= i;
		powx *= x;
	}

	return (res * resi);
}


double ceil(double x)
{
#ifdef __IEEE754_CEIL
	return __ieee754_ceil(x);
#else
	double ipart, fpart;

	if (isnan(x) != 0) {
		return NAN;
	}

	fpart = modf(x, &ipart);

	if ((x > 0.0) && ((fpart + x) != x)) {
		ipart += 1.0;
	}

	return ipart;
#endif
}


float ceilf(float x)
{
#ifdef __IEEE754_CEILF
	return __ieee754_ceilf(x);
#else
	return (float)ceil(x);
#endif
}


double floor(double x)
{
#ifdef __IEEE754_FLOOR
	return __ieee754_floor(x);
#else
	double ipart, fpart;

	if (isnan(x) != 0) {
		return NAN;
	}

	fpart = modf(x, &ipart);

	if ((x < 0.0) && ((fpart + x) != x)) {
		ipart -= 1.0;
	}

	return ipart;
#endif
}


float floorf(float x)
{
#ifdef __IEEE754_FLOORF
	return __ieee754_floorf(x);
#else
	return (float)floor(x);
#endif
}


double fmod(double number, double denom)
{
	double result, tquot;

	if (isnan(number) != 0 || isnan(denom) != 0) {
		return NAN;
	}

	if ((denom == 0.0) || (isinf(number) != 0)) {
		errno = EDOM;
		return NAN;
	}

	if (((number == 0.0) && (denom != 0.0)) ||
			((isinf(number) == 0) && (isinf(denom) != 0))) {
		return number;
	}

	modf(number / denom, &tquot);
	result = tquot * denom;

	return number - result;
}


double round(double x)
{
#ifdef __IEEE754_ROUND
	return __ieee754_round(x);
#else
	double ret, frac;

	if (isnan(x) != 0) {
		return NAN;
	}

	frac = modf(x, &ret);

	if (frac >= 0.5) {
		ret += 1.0;
	}
	else if (frac <= -0.5) {
		ret -= 1.0;
	}

	return ret;
#endif
}


float roundf(float x)
{
#ifdef __IEEE754_ROUNDF
	return __ieee754_roundf(x);
#else
	return (float)round(x);
#endif
}


double trunc(double x)
{
#ifdef __IEEE754_TRUNC
	return __ieee754_trunc(x);
#else
	double ret;

	if (isnan(x) != 0) {
		return NAN;
	}

	modf(x, &ret);

	return ret;
#endif
}


float truncf(float x)
{
#ifdef __IEEE754_TRUNCF
	return __ieee754_truncf(x);
#else
	return (float)trunc(x);
#endif
}


double fabs(double x)
{
#ifdef __IEEE754_FABS
	return __ieee754_fabs(x);
#else
	if (isnan(x) != 0) {
		return NAN;
	}

	conv_t *conv = (conv_t *)&x;
	conv->i.sign = 0;

	return x;
#endif
}


float fabsf(float x)
{
#ifdef __IEEE754_FABSF
	return __ieee754_fabsf(x);
#else
	return (float)fabs(x);
#endif
}
