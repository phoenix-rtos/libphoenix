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


float modff(float x, float *intpart)
{
	double ret, tmp;

	ret = modf(x, &tmp);
	*intpart = tmp;

	return ret;
}
