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

#include ARCH
#include "../math.h"
#include "../limits.h"
#include "common.h"

#if 0
/* Uses a^x = e^(x * ln(a)) identity */
double pow(double base, double exponent) /* TODO testing */
{
	double res;
	int e;

	if (base == 0 && exponent == 0) {
		/* TODO: errno EDOM */
		return NAN;
	}

	if (base == 0)
		return 0.0;

	if (exponent == 0)
		return 1.0;

	if (base < 0) {
		if (!isInteger(exponent)) {
			/* TODO: errno EDOM */
			return NAN;
		}

		res = 1.0;

		while (exponent != 0.0) {
			if (exponent > INT_MAX)
				e = INT_MAX;
			else if (exponent < INT_MIN)
				e = INT_MIN;
			else
				e = (int)exponent;

			res *= quickPow(base, e);
			exponent -= e;
		}
	}
	else {
		exponent *= log(base);
		res = exp(exponent);
	}

	return res;
}


/* Calculates value of square root using Newtons method to
 * solve f(y) = y^2 - x = 0 for y, where x is input value */
double sqrt(double x) /* TODO testing */
{
	double y;
	int i;

	if (x < 0) {
		/* TODO: errno EDOM */
		return NAN;
	}

	y = (x > 1) ? x / 2 : x * 2;

	if (y == 0)
		return 0;

	for (i = 0; i < 12; ++i) /* TODO pick number of iterations */
		y = y - ((y * y - x) / (2 * y));

	return y;
}
#endif
