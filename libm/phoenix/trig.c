/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * cos, sin, tan, acos, asin, atan
 *
 * Copyright 2017, 2018, 2026 Phoenix Systems
 * Author: Aleksander Kaminski, Jakub Smolaga
 *
 * This file is part of Phoenix-RTOS.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <math.h>
#include <errno.h>


/* Calculates value of cosine using Maclaurin series via Horner's method. */
double cos(double x)
{
	double res, xpow;

	if (isnan(x) != 0) {
		return NAN;
	}

	if (isinf(x) != 0) {
		errno = EDOM;
		return NAN;
	}

	/* Normalize argument to -2*PI < x < 2*PI */
	x = fmod(x, 2.0 * M_PI);

	/* Normalize further to -PI < x < PI */
	if (x > M_PI) {
		x -= 2.0 * M_PI;
	}
	else if (x < -M_PI) {
		x += 2.0 * M_PI;
	}

	if (x > M_PI_2) {
		return -sin(x - M_PI_2);
	}
	else if (x < -M_PI_2) {
		return sin(x + M_PI_2);
	}

	xpow = x * x;

	res = 0x1.e542ba4020225p-62;              /* 1/20! */
	res = res * xpow - 0x1.6827863b97d97p-53; /* 1/18! */
	res = res * xpow + 0x1.ae7f3e733b81fp-45; /* 1/16! */
	res = res * xpow - 0x1.93974a8c07c9dp-37; /* 1/14! */
	res = res * xpow + 0x1.1eed8eff8d898p-29; /* 1/12! */
	res = res * xpow - 0x1.27e4fb7789f5cp-22; /* 1/10! */
	res = res * xpow + 0x1.a01a01a01a01ap-16; /* 1/8!  */
	res = res * xpow - 0x1.6c16c16c16c17p-10; /* 1/6!  */
	res = res * xpow + 0x1.5555555555555p-5;  /* 1/4!  */
	res = res * xpow - 0x1p-1;                /* 1/2!  */
	res = res * xpow + 0x1p0;                 /* 1/0!  */

	return res;
}


float cosf(float x)
{
	return (float)cos((double)x);
}


/* Calculates value of sine using Maclaurin series via Horner's method. */
double sin(double x)
{
	double res, xpow;

	if (isnan(x) != 0) {
		return NAN;
	}

	if (isinf(x) != 0) {
		errno = EDOM;
		return NAN;
	}

	if (x == 0.0) {
		return x;
	}

	/* Normalize argument to -2*PI < x < 2*PI */
	x = fmod(x, 2.0 * M_PI);

	/* Normalize further to -PI < x < PI */
	if (x > M_PI) {
		x -= 2 * M_PI;
	}
	else if (x < -M_PI) {
		x += 2 * M_PI;
	}

	if (x > M_PI_2) {
		return cos(x - M_PI_2);
	}
	else if (x < -M_PI_2) {
		return -cos(x + M_PI_2);
	}

	xpow = x * x;

	res = 0x1.71b8ef6dcf572p-66;              /* 1/21! */
	res = res * xpow - 0x1.2f49b46814157p-57; /* 1/19! */
	res = res * xpow + 0x1.952c77030ad4ap-49; /* 1/17! */
	res = res * xpow - 0x1.ae7f3e733b81fp-41; /* 1/15! */
	res = res * xpow + 0x1.6124613a86d09p-33; /* 1/13! */
	res = res * xpow - 0x1.ae64567f544e4p-26; /* 1/11! */
	res = res * xpow + 0x1.71de3a556c734p-19; /* 1/9!  */
	res = res * xpow - 0x1.a01a01a01a01ap-13; /* 1/7!  */
	res = res * xpow + 0x1.1111111111111p-7;  /* 1/5!  */
	res = res * xpow - 0x1.5555555555555p-3;  /* 1/3!  */
	res = res * xpow + 0x1.0p0;               /* 1/1!  */

	return res * x;
}


float sinf(float x)
{
	return (float)sin((double)x);
}


double tan(double x)
{
	double c;

	if (isnan(x) != 0) {
		return NAN;
	}

	if (isinf(x) != 0) {
		errno = EDOM;
		return NAN;
	}

	c = cos(x);

	if (c != 0.0) {
		return (sin(x) / c);
	}
	else {
		return 0.0;
	}
}


float tanf(float x)
{
	return (float)tan((double)x);
}


/* Calculates value of arc cosine using secant method */
double acos(double x)
{
	double xa = 0, xb = M_PI, ya, yb, t;
	int i;

	if (isnan(x) != 0) {
		return NAN;
	}

	if ((x > 1.0) || (x < -1.0)) {
		errno = EDOM;
		return NAN;
	}

	for (i = 0; i < 16; ++i) {
		ya = cos(xa) - x;
		yb = cos(xb) - x;

		t = ya - yb;

		if (t == 0.0) {
			break;
		}

		t = ((ya * xb) - (yb * xa)) / t;
		xa = xb;
		xb = t;
	}

	return xb;
}


float acosf(float x)
{
	return (float)acos((double)x);
}


/* Calculates value of arc sine using asin(x) = pi/2 - acos(x) relationship. */
double asin(double x)
{
	if (isnan(x) != 0) {
		return NAN;
	}

	if (x == 0.0) {
		return x;
	}

	return M_PI_2 - acos(x);
}


float asinf(float x)
{
	return (float)asin((double)x);
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
	int i, s;

	if (isnan(x) != 0) {
		return NAN;
	}

	s = (x < 0.0) ? -1 : 1;
	x *= (double)s;
	h = x / 2;

	if (x == 0.0) {
		return x;
	}

	if (x > 1.0) {
		return ((M_PI_2 - atan(1.0 / x)) * (double)s);
	}

	for (i = 0, res = 0.0; i < sizeof(atan_wi) / sizeof(atan_wi[0]); ++i) {
		a = (h * atan_xi[i]) + h;
		res += atan_wi[i] / ((a * a) + 1);
	}

	return (res * h * (double)s);
}


float atanf(float x)
{
	return (float)atan((double)x);
}


double atan2(double y, double x)
{
	if ((isnan(y) != 0) || (isnan(x) != 0)) {
		return NAN;
	}

	if (y == 0.0) {
		if (x < 0.0 || signbit(x) != 0) {
			// x is negative or -0.0
			return (signbit(y) != 0) ? -M_PI : M_PI;
		}
		else {
			// x is positive or +0.0
			return y;
		}
	}

	if ((isinf(y) != 0) && (isinf(x) != 0)) {
		int sy = signbit(y);
		int sx = signbit(x);

		if ((sy == 0) && (sx != 0)) {
			return M_PI_4 * 3;
		}
		else if ((sy != 0) && (sx != 0)) {
			return -M_PI_4 * 3;
		}
		else if ((sy == 0) && (sx == 0)) {
			return M_PI_4;
		}
		else if ((sy != 0) && (sx == 0)) {
			return -M_PI_4;
		}
	}

	if (x > 0.0) {
		return atan(y / x);
	}
	else if (x < 0.0) {
		if (y >= 0.0) {
			return atan(y / x) + M_PI;
		}

		return atan(y / x) - M_PI;
	}

	if (y > 0.0) {
		return M_PI_2;
	}
	else if (y < 0.0) {
		return -M_PI_2;
	}

	return 0.0;
}


float atan2f(float y, float x)
{
	return (float)atan2((double)y, (double)x);
}
