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


double trunc(double x)
{
	double ret;

	if (isnan(x) != 0) {
		return NAN;
	}

	modf(x, &ret);

	return ret;
}
