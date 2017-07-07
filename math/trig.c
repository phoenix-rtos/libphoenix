/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * cos, sin, acos, asin, atan
 *
 * Copyright 2017 Phoenix Systems
 * Author: Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include ARCH
#include "consts.h"


static double trig_normalizeArg(double x) /* TODO testing */
{
	int k;

	if (x < -M_PI || x > M_PI) {
		k = (int)(x / (2 * M_PI));
		x = x - k * 2 * M_PI;
	}

	return x;
}


/* Calculates value of cosine using Maclaurin series. */
double cos(double x) /* TODO testing */
{
	int i, strong;
	double res, xn, xpow;

	x = trig_normalizeArg(x);

	if (x == 0)
		return 0.0;
	else if (x == M_PI_2)
		return 1.0;

	res = 1;
	xpow = x * x;
	xn = xpow;
	strong = 2;

	for (i = 0; i < 4; ++i) { /* TODO pick number of iterations */
		if (i & 1)
			res -= xn / strong;
		else
			res += xn / strong;

		xn *= xpow;
		strong = strong * (2 * i + 3) * (2 * i + 4);
	}

	return res;
}


/* Calculates value od sine using sin(x) = cos(x + pi/2) relationship. */
double sin(double x) /* TODO testing */
{
	return cos(x + M_PI_2);
}


/* Calculates arcus cosine value using Newton method solving cos(y) - x = 0 for y */
double acos(double x) /* TODO testing */
{
	double y;
	int i;

	if (x > 1.0 || x < -1.0) {
		/* TODO: errno EDOM */
		return NAN;
	}

	y = M_PI_2;

	for (i = 0; i < 10; ++i) /* TODO pick number of iterations */
		y = y + ((cos(y) - x) / sin(y));

	return y;
}


/* Calculates value od sine using asin(x) = pi/2 - acos(x) relationship. */
double asin(double x) /* TODO testing */
{
	return M_PI_2 - acos(x);
}

#if 0
double atan(double x)
{
	return 0;
}


double atan2(double y, double x)
{
	return 0;
}
#endif
