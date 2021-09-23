/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * ceil, copysign, fabs, floor, fmod,  modf, modff, round, signbit, trunc
 *
 * Copyright 2021 Phoenix Systems
 * Authors: Aleksander Kaminski, Mateusz Niewiadomski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <math.h>
#include <stdint.h>
#include "common.h"


double modf(double x, double* intpart)
{
	conv_t *conv = (conv_t *)&x;
	double tmp = x;
	int exp = conv->i.exponent - 1023;
	uint64_t m, mask = 0xfffffffffffffLL;

	if (exp > 52) {
		*intpart = x;
		return conv->i.sign? -0.0 : 0.0;
	}
	else if (exp < 0) {
		*intpart = conv->i.sign? -0.0 : 0.0;
		return x;
	}

	conv->i.mantisa = conv->i.mantisa & ~(mask >> exp);
	*intpart = x;
	x = tmp;

	m = conv->i.mantisa;
	m &= mask >> exp;

	if (m == 0)
		return 0.0;

	conv->i.mantisa = m & mask;
	normalizeSub(&x, &exp);

	conv->i.exponent = exp + 1023;

	return x;
}

float modff(float x, float* intpart)
{
	double ret, tmp;

	ret = modf(x, &tmp);
	*intpart = tmp;

	return ret;
}


double ceil(double x)
{
	double ipart, fpart;

	fpart = modf(x, &ipart);

	if (x > 0 && fpart + x != x)
		ipart += 1.0;

	return ipart;
}


double floor(double x)
{
	double ipart, fpart;

	fpart = modf(x, &ipart);

	if (x < 0 && fpart + x != x)
		ipart -= 1.0;

	return ipart;
}


double fmod(double number, double denom)
{
	double result, tquot;

	if (denom == 0.0 || denom == -0.0)
		return NAN;

	if (denom == INFINITY || denom == -INFINITY)
		return number;

	modf(number / denom, &tquot);
	result = tquot * denom;

	return number - result;
}


double round(double x)
{
	double ret, frac;

	frac = modf(x, &ret);

	if (frac >= 0.5)
		ret += 1.0;
	else if (frac <= -0.5)
		ret -= 1.0;

	return ret;
}


double trunc(double x)
{
	double ret;

	modf(x, &ret);

	return ret;
}


double fabs(double x)
{
	conv_t *conv = (conv_t *)&x;

	conv->i.sign = 0;

	return x;
}


int signbit(double x)
{
	return ((0 < x) - (x < 0));
}


double copysign(double x, double y)
{
	return y == 0.0 ? fabs(x) : fabs(x)*y/fabs(y);
}
