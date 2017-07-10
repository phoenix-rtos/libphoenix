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


/* Uses quick powering method when base is positive and exponent is integer.
 * In other case Maclaurin series are used */
double pow(double base, double exponent) /* TODO testing */
{
	double loga, logapow, exppow, result;
	unsigned int strong, i, exp = (unsigned int)exponent;

	if (base < 0 && !isInteger(exponent)) {
		/* TODO: errno EDOM */
		return NAN;
	}

	if (base == 0 && exponent == 0) {
		/* TODO: errno EDOM */
		return NAN;
	}

	if (base == 0)
		return 0.0;

	if (exponent == 0)
		return 1.0;

	result = 1;

	if (exponent > 0 && isInteger(exponent) && exponent <= UINT32_MAX) {
		while (exp > 0) {
			if (exp & 1) {
				if (exponent > 0)
					result *= base;
				else
					result /= base;
			}
			base *= base;
			exp >>= 1;
		}
	}
	else {
		loga = log(base);
		logapow = loga;
		exppow = exponent;
		strong = 2;

		for (i = 0; i < 9; ++i) { /* TODO pick number of iterations */
			result += logapow * exppow / strong;

			logapow = logapow * loga;
			exppow = exppow * exponent;
			strong = strong * (i + 3);
		}
	}

	return result;
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
