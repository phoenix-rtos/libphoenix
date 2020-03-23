/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * pow, sqrt
 *
 * Copyright 2017 Phoenix Systems
 * Author: Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <math.h>
#include <limits.h>
#include <errno.h>
#include "common.h"


/* Uses a^x = e^(x * ln(a)) identity */
double pow(double base, double exponent)
{
	double res = 1.0;
	int s = 1;

	if (base == 0.0 || base == -0.0) {
		if (exponent == 0.0 || exponent == -0.0) {
			return 1.0;
		}
		else if (exponent < 0.0) {
			if (base == 0.0)
				return INFINITY;
			else if (base == -0.0)
				return -INFINITY;
		}

		return 0.0;
	}
	else if (exponent == 0.0 || exponent == -0.0) {
		return 1.0;
	}

	if (base < 0.0) {
		if (!isInteger(exponent)) {
			errno = EDOM;
			return NAN;
		}

		s = (fmod(exponent, 2) > 0.0) ? -1 : 1;
		base = -base;
	}

	if (isInteger(exponent) && exponent <= INT_MAX && exponent >= INT_MIN)
		return quickPow(base, (int)exponent);

	exponent *= log(base);
	res = s * exp(exponent);

	return res;
}

/* Uses reciprocal square root method: */
double sqrt(double x)
{
	if (x < 0.0) {
		errno = EDOM;
		return -NAN;
	}

	if (x == 0.0 || x == -0.0)
		return x;

	double xn = (double)(1.0f / (float)x);

	/* IEEE-754 compliant: */
	conv_t *init = (conv_t *)&xn;

	/* +Infinity : */
	if (init->i.mantisa == 0 && init->i.exponent == 0x7FF)
		return x;

	/* Subnormals: */
	if (init->i.exponent == 0)
		init->i.exponent = 0x1;


	/* Initial guess: */
	init->i.mantisa = (init->i.mantisa >> 1);

	if (init->i.exponent & 0x1) {
		init->i.exponent = (init->i.exponent >> 1) + 0x200;
	}
	else {
		init->i.exponent = (init->i.exponent >> 1) + 0x1FF;
		init->i.mantisa |= (0x1ull << 51);
	}

	int i;
	/* Reciprocal sqare root iters (avoiding division): */
	for (i = 0; i < 4; ++i)
		xn = xn * (1.5 -  0.5 * x * xn * xn);

	return xn * x;
}
