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
 * This file is part of Phoenix-RTOS.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
 
#include <math.h>


float fabsf(float x)
{
	return (float)fabs(x);
}
