/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * math.h
 *
 * Copyright 2017 Phoenix Systems
 * Author: Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_MATH_H_
#define _LIBPHOENIX_MATH_H_


#include <stdarg.h>
#include <math/consts.h>


#define _HUGE_ENUF  1e+300  // _HUGE_ENUF*_HUGE_ENUF must overflow

#define HUGE_VALF  ((float)INFINITY)
#define HUGE_VALL  ((long double)INFINITY)
#define isnan(n) ((n != n) ? 1 : 0)
#define isinf(x) __builtin_isinf(x)
#define signbit(x) ((0 < x) - (x < 0))
#define strtof strtod

/* Trigonometric functions */


/* Returns the cosine of an angle of x radians. */
extern double cos(double x);


/* Returns the sine of an angle of x radians. */
extern double sin(double x);


/* Returns the tangent of an angle of x radians. */
extern double tan(double x);


/* Returns the principal value of the arc cosine of x, expressed in radians. */
extern double acos(double x);


/* Returns the principal value of the arc sine of x, expressed in radians. */
extern double asin(double x);


/* Returns the principal value of the arc tan of x, expressed in radians. */
extern double atan(double x);


/* Returns the principal value of the arc tangent of y/x, expressed in radians. */
extern double atan2(double y, double x);


/* Hyperbolic functions */


/* Returns the hyperbolic cosine of x. */
extern double cosh(double x);


/* Returns the hyperbolic sine of x. */
extern double sinh(double x);


/* Returns the hyperbolic tangent of x. */
extern double tanh(double x);


/* Exponential and logarithmic functions */


/* Returns the base-e exponential function of x, which is e raised to the power x: e^x. */
extern double exp(double x);


/* Breaks the floating point number x into its binary significand
 * (a floating point with an absolute value between 0.5(included) and 1.0(excluded))
 * and an integral exponent for 2. */
extern double frexp(double x, int* exp);


/* Returns the result of multiplying x (the significand) by 2 raised to the power of exp (the exponent). */
extern double ldexp(double x, int exp);


/* Returns the natural logarithm of x. */
extern double log(double x);


/* Returns the common (base-2) logarithm of x. */
extern double log2(double x);


/* Returns the common (base-10) logarithm of x. */
extern double log10(double x);


/* Breaks x into an integral and a fractional part. */
extern double modf(double x, double* intpart);
extern float modff(float x, float* intpart);


/* Power functions */


/* Returns base raised to the power exponent. */
extern double pow(double base, double exponent);


/* Returns the square root of x. */
extern double sqrt(double x);


/* Rounding and remainder functions */


/* Rounds x upward, returning the smallest integral value that is not less than x. */
extern double ceil(double x);


/* Rounds x downward, returning the largest integral value that is not greater than x. */
extern double floor(double x);


/* Returns the floating-point remainder of numer/denom (rounded towards zero). */
extern double fmod(double numer, double denom);


/* Return the integral value nearest to x */
extern double round(double x);


/* Rounds x toward zero, returning the nearest integral value that is not larger in magnitude than x. */
extern double trunc(double x);


/* Miscellaneous */


/* Returns the absolute value of x: |x|. */
extern double fabs(double x);


/* C99 extensions */
#define cosf(x) cos(x)
#define sinf(x) sin(x)
#define tanf(x) tan(x)
#define acosf(x) acos(x)
#define asinf(x) asin(x)
#define atanf(x) atan(x)
#define atan2f(y, x) atan2(y, x)
#define coshf(x) cosh(x)
#define sinhf(x) sinh(x)
#define tanhf(x) tanh(x)
#define expf(x) exp(x)
#define frexpf(x, exp) frexp(x, exp)
#define ldexpf(x, exp) ldexp(x, exp)
#define logf(x) log(x)
#define log10f(x) log10(x)
#define powf(base, exponent) pow(base, exponent)
#define sqrtf(x) sqrt(x)
#define roundf(x) round(x)
#define ceilf(x) ceil(x)
#define floorf(x) floor(x)
#define fmodf(numer, denom) fmod(numer, denom)
#define truncf(x) trunc(x)
#define fabsf(x) fabs(x)


#endif
