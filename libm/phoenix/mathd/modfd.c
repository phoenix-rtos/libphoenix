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
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <math.h>
#include "common.h"

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
