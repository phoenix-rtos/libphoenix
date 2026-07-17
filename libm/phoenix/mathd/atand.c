/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * cos, sin, tan, acos, asin, atan
 *
 * Copyright 2017, 2018, 2026 Phoenix Systems
 * Author: Aleksander Kaminski, Jakub Smolaga
 *
 * This file is part of Phoenix-RTOS.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
 
#include <math.h>


static const double atan_wi[] = { 0.2152638534631578, 0.2152638534631578, 0.2051984637212956,
	0.2051984637212956, 0.1855383974779378, 0.1855383974779378, 0.1572031671581935,
	0.1572031671581935, 0.1215185706879032, 0.1215185706879032, 0.0801580871597602,
	0.0801580871597602, 0.0351194603317519, 0.0351194603317519 };


static const double atan_xi[] = { -0.1080549487073437, 0.1080549487073437, -0.3191123689278897,
	0.3191123689278897, -0.5152486363581541, 0.5152486363581541, -0.6872929048116855,
	0.6872929048116855, -0.8272013150697650, 0.8272013150697650, -0.9284348836635735,
	0.9284348836635735, -0.9862838086968123, 0.9862838086968123 };


double atan(double x)
{
	double res = 1.0, h, a;
	int i, s;

	if (isnan(x) != 0) {
		return NAN;
	}

	s = (x < 0.0) ? -1 : 1;
	x *= (double)s;
	h = x / 2;

	if (x == 0.0) {
		return x;
	}

	if (x > 1.0) {
		return ((M_PI_2 - atan(1.0 / x)) * (double)s);
	}

	for (i = 0, res = 0.0; i < sizeof(atan_wi) / sizeof(atan_wi[0]); ++i) {
		a = (h * atan_xi[i]) + h;
		res += atan_wi[i] / ((a * a) + 1);
	}

	return (res * h * (double)s);
}
