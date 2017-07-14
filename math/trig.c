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
#include "../math.h"


/* Calculates value of cosine using Maclaurin series. */
double cos(double x)
{
	int i;
	double res, xn, xpow, strong, sign = 1.0;

	/* Normalize argument to -2*PI < x < 2*PI */
	x = fmod(x, 2.0 * M_PI);

	/* Normalize further to -PI < x < PI */
	if (x > M_PI) {
		x -= M_PI;
		sign = -1.0;
	}
	else if (x < -M_PI) {
		x += M_PI;
		sign = -1.0;
	}

	res = 1.0;
	xpow = x * x;
	xn = xpow;
	strong = 2.0;

	for (i = 0; i < 13; ++i) {
		if (i & 1)
			res += xn / strong;
		else
			res -= xn / strong;

		xn *= xpow;
		strong = strong * (2 * i + 3) * (2 * i + 4);
	}

	return sign *res;
}


/* Calculates value od sine using sin(x) = cos(x - pi/2) relationship. */
double sin(double x)
{
	return cos(x - M_PI_2);
}

#if 0
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


double atan(double x)
{
	return 0;
}


double atan2(double y, double x)
{
	return 0;
}
#endif
