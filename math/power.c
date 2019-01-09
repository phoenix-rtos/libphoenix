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


/* Uses sqrt(x) = x^(1/2) indentity */
double sqrt(double x)
{
	if (x < 0.0) {
		errno = EDOM;
		return -NAN;
	}

	if (x == 0.0 || x == -0.0)
		return 0.0;

	return pow(x, 0.5);
}
