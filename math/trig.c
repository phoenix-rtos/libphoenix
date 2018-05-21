/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * cos, sin, tan, acos, asin, atan
 *
 * Copyright 2017, 2018 Phoenix Systems
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
	double res, xn, xpow, strong;

	/* Normalize argument to -2*PI < x < 2*PI */
	x = fmod(x, 2.0 * M_PI);

	/* Normalize further to -PI < x < PI */
	if (x > M_PI)
		x -= 2 * M_PI;
	else if (x < -M_PI)
		x += 2 * M_PI;

	if (x > M_PI_2)
		return -sin(x - M_PI_2);
	else if (x < -M_PI_2)
		return sin(x + M_PI_2);

	res = 1.0;
	xpow = x * x;
	xn = xpow;
	strong = 2.0;

	for (i = 0; i < 10; ++i) {
		if (i & 1)
			res += xn / strong;
		else
			res -= xn / strong;

		xn *= xpow;
		strong = strong * (2 * i + 3) * (2 * i + 4);
	}

	return res;
}


/* Calculates value of sine using Maclaurin series. */
double sin(double x)
{
	int i;
	double res, xn, xpow, strong;

	/* Normalize argument to -2*PI < x < 2*PI */
	x = fmod(x, 2.0 * M_PI);

	/* Normalize further to -PI < x < PI */
	if (x > M_PI)
		x -= 2 * M_PI;
	else if (x < -M_PI)
		x += 2 * M_PI;

	if (x > M_PI_2)
		return cos(x - M_PI_2);
	else if (x < -M_PI_2)
		return -cos(x + M_PI_2);

	res = x;
	xpow = x * x;
	xn = xpow * x;
	strong = 6.0;

	for (i = 0; i < 10; ++i) {
		if (i & 1)
			res += xn / strong;
		else
			res -= xn / strong;

		xn *= xpow;
		strong = strong * (2 * i + 4) * (2 * i + 5);
	}

	return res;
}


double tan(double x)
{
	double c = cos(x);

	if (c > 0.0 || c < 0.0)
		return sin(x) / c;

	/* TODO errno EDOM */
	return NAN;
}


/* Calculates arcus cosine value using Newton method solving cos(y) - x = 0 for y */
double acos(double x)
{
	double y;
	int i;

	if (x > 1.0 || x < -1.0) {
		/* TODO: errno EDOM */
		return NAN;
	}

	y = M_PI_2;

	for (i = 0; i < 10; ++i)
		y = y + ((cos(y) - x) / sin(y));

	return y;
}


/* Calculates value od sine using asin(x) = pi/2 - acos(x) relationship. */
double asin(double x)
{
	return M_PI_2 - acos(x);
}


static const double atan_wi[] = { 0.4179591836734694, 0.3818300505051189, 0.3818300505051189,
	0.2797053914892766, 0.2797053914892766, 0.1294849661688697, 0.1294849661688697 };


static const double atan_xi[] = { 0.0000000000000000, 0.4058451513773972, -0.4058451513773972,
	-0.7415311855993945, 0.7415311855993945, -0.9491079123427585, 0.9491079123427585 };


double atan(double x)
{
	double res = 1.0, h, a;
	int i, s = (x < 0.0) ? -1 : 1;

	x *= s;
	h = x / 2;

	if (!(x > 0.0))
		return 0.0;

	if (x > 1.0)
		return M_PI_2 * s - atan(1.0 / x);

	for (i = 0, res = 0.0; i < 7; ++i) {
		a = h * atan_xi[i] + h;
		res += atan_wi[i] / (a * a + 1);
	}

	return res * h * s;
}


double atan2(double y, double x)
{
	if (x > 0) {
		return atan(y / x);
	}
	else if (x < 0) {
		if (y >= 0)
			return atan(y / x) + M_PI;

		return atan(y / x) - M_PI;
	}

	if (y > 0)
		return M_PI_2;
	else if (y < 0)
		return -M_PI_2;

	return NAN; /* TODO errno */
}
