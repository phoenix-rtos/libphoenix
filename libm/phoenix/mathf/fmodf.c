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


float fmodf(float x, float y)
{
	return (float)fmod((double)x, (double)y);
}
