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
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <math.h>
#include <errno.h>
#include <limits.h>
#include <float.h>
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
