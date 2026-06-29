/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Compatibility source file for libmcs header file
 *
 * Copyright 2025 Phoenix Systems
 * Author: Mikolaj Matalowski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */
#include <math.h>


int __fpclassifyf(float x)
{
	return __builtin_fpclassify(FP_NAN, FP_INFINITE, FP_NORMAL, FP_SUBNORMAL, FP_ZERO, x);
}


int __fpclassifyd(double x)
{
	return __builtin_fpclassify(FP_NAN, FP_INFINITE, FP_NORMAL, FP_SUBNORMAL, FP_ZERO, x);
}


float nanf(const char *)
{
	return __builtin_nanf("");
}


int __signbitf(float x)
{
	return __builtin_signbitf(x);
}


int __signbitd(double x)
{
	return __builtin_signbit(x);
}


const float __inff = __builtin_inff();
const double __infd = __builtin_inf();
