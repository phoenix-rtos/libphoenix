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

#include <math.h>
#include <errno.h>


/* Calculates value of cosine using Maclaurin series. */
double cos(double x)
{
	int i;
	double res, xn, xpow, factorial;

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
	factorial = 2.0;

	for (i = 0; i < 10; ++i) {
		if (i & 1)
			res += xn / factorial;
		else
			res -= xn / factorial;

		xn *= xpow;
		factorial = factorial * (2 * i + 3) * (2 * i + 4);
	}

	return res;
}


/* Calculates value of sine using Maclaurin series. */
double sin(double x)
{
	int i;
	double res, xn, xpow, factorial;

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
	factorial = 6.0;

	for (i = 0; i < 10; ++i) {
		if (i & 1)
			res += xn / factorial;
		else
			res -= xn / factorial;

		xn *= xpow;
		factorial = factorial * (2 * i + 4) * (2 * i + 5);
	}

	return res;
}


double tan(double x)
{
	double c = cos(x);

	if (c > 0.0 || c < 0.0)
		return sin(x) / c;

	errno = EDOM;
	return NAN;
}


/* Calculates value of arc cosine using secant method */
double acos(double x)
{
	double xa = 0, xb = M_PI, ya, yb, t;
	int i;

	if (x > 1.0 || x < -1.0) {
		errno = EDOM;
		return NAN;
	}

	for (i = 0; i < 16; ++i) {
		ya = cos(xa) - x;
		yb = cos(xb) - x;

		t = ya - yb;

		if (t == 0.0 || t == -0.0)
			break;

		t = (ya * xb - yb * xa) / t;
		xa = xb;
		xb = t;
	}

	return xb;
}


/* Calculates value of arc sine using asin(x) = pi/2 - acos(x) relationship. */
double asin(double x)
{
	return M_PI_2 - acos(x);
}


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
	int i, s = (x < 0.0) ? -1 : 1;

	x *= s;
	h = x / 2;

	if (x <= 0.0)
		return 0.0;

	if (x > 1.0)
		return (M_PI_2 - atan(1.0 / x)) * s;

	for (i = 0, res = 0.0; i < sizeof(atan_wi) / sizeof(atan_wi[0]); ++i) {
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

	errno = EDOM;
	return NAN;
}
