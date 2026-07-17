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


float modff(float x, float *intpart)
{
	double ret, tmp;

	ret = modf((double)x, &tmp);
	*intpart = tmp;

	return (float)ret;
}
