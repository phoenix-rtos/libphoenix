/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Copyright 2017 Phoenix Systems
 * Author: Aleksander Kaminski
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <math.h>


double floor(double x)
{
	double ipart, fpart;

	if (isnan(x) != 0) {
		return NAN;
	}

	fpart = modf(x, &ipart);

	if ((x < 0.0) && ((fpart + x) != x)) {
		ipart -= 1.0;
	}

	return ipart;
}
