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

#include <arch.h> /* Needed for __ieee754_sqrt */
#include <math.h>
#include <float.h>
#include <limits.h>
#include <errno.h>
#include <stdlib.h>
#include "common.h"


/* Uses a^x = e^(x * ln(a)) identity */
double pow(double base, double exponent)
{
	double res = 1.0;
	int s = 1;

	if ((base == 1.0) || (exponent == 0.0) ||
			((base == -1.0) && (isinf(exponent) != 0))) {
		return 1.0;
	}

	if ((isnan(base) != 0) || (isnan(exponent) != 0)) {
		return NAN;
	}

	if ((isinf(base) != 0) && (exponent != 0.0)) {
		if (signbit(base) != 0) {
			if (exponent < 0.0) {
				return (fmod(fabs(exponent), 2.0) == 1.0) ? -0.0 : 0.0;
			}
			else {
				return (fmod(fabs(exponent), 2.0) == 1.0) ? -INFINITY : INFINITY;
			}
		}
		else {
			return (exponent < 0.0) ? 0.0 : INFINITY;
		}
	}

	if (base == 0.0) {
		if ((fmod(fabs(exponent), 2.0) == 1.0)) {
			if (exponent > 0.0) {
				return base;
			}
			else {
				errno = ERANGE;
				return (signbit(base) != 0) ? -HUGE_VAL : HUGE_VAL;
			}
		}
		else if (exponent < 0.0) {
			errno = ERANGE;
			return HUGE_VAL;
		}
	}

	if (base < 0.0) {
		if (!isInteger(exponent)) {
			errno = EDOM;
			return NAN;
		}

		s = (int)((fmod(exponent, 2) > 0.0) ? -1 : 1);
		base = -base;
	}

	if (isInteger(exponent) && (exponent <= INT_MAX) && (exponent >= INT_MIN)) {
		return quickPow(base, (int)exponent);
	}

	exponent *= log(base);
	res = (double)s * exp(exponent);

	/* ia32 use 80-bit extended precision registers so isinf() may not be true */
	if ((isinf(res) != 0) || (fabs(res) > DBL_MAX)) {
		errno = ERANGE;
	}

	return res;
}


double sqrt(double x)
{
	if (isnan(x) != 0) {
		return NAN;
	}

	if (x < 0.0) {
		errno = EDOM;
		return NAN;
	}

	if ((x == 0.0) || (x == INFINITY)) {
		return x;
	}

#ifdef __IEEE754_SQRT
	return __ieee754_sqrt(x);
#else
	/* Use reciprocal square root method: */
	double xn = 1.0 / x;

	/* IEEE-754 compliant: */
	conv_t *init = (conv_t *)&xn;

	/* +Infinity : */
	if ((init->i.mantisa == 0) && (init->i.exponent == 0x7FF)) {
		return x;
	}

	/* Subnormals: */
	if (init->i.exponent == 0) {
		init->i.exponent = 0x1;
	}

	/* Initial guess: */
	init->i.mantisa = (init->i.mantisa >> 1);

	if (init->i.exponent & 0x1) {
		init->i.exponent = (init->i.exponent >> 1) + 0x200;
	}
	else {
		init->i.exponent = (init->i.exponent >> 1) + 0x1FF;
		init->i.mantisa |= (0x1ull << 51);
	}

	/* Reciprocal sqare root iters (avoiding division): */
	for (int i = 0; i < 4; ++i) {
		xn = xn * (1.5 - (0.5 * x * xn * xn));
	}

	return (xn * x);
#endif
}


float sqrtf(float x)
{
#ifdef __IEEE754_SQRTF
	if (isnan(x) != 0) {
		return NAN;
	}

	if (x < 0.0f) {
		errno = EDOM;
		return NAN;
	}

	if ((x == 0.0f) || (x == INFINITY)) {
		return x;
	}

	return __ieee754_sqrtf(x);
#else
	return (float)sqrt(x);
#endif
}
