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

#include "../math.h"
#include "../limits.h"
#include "common.h"


/* Uses a^x = e^(x * ln(a)) identity */
double pow(double base, double exponent)
{
	double res = 1.0;
	int s = 1;

	if (base == 0.0) {
		if (exponent == 0.0) {
			/* TODO: errno EDOM */
			return NAN;
		}
		else {
			return 0.0;
		}
	}
	else if (exponent == 0.0)
		return 1.0;

	if (base < 0.0) {
		if (!isInteger(exponent))
			return NAN; /* TODO: errno EDOM */
		else
			s = (fmod(exponent, 2) > 0.0) ? -1 : 1;

		base = -base;
	}

	exponent *= log(base);
	res = s * exp(exponent);

	return res;
}


/* Uses sqrt(x) = x^(1/2) indentity */
double sqrt(double x)
{
	if (x < 0.0) {
		/* TODO: errno EDOM */
		return -NAN;
	}
	else if (x == 0.0) {
		return 0.0;
	}

	return pow(x, 0.5);
}
