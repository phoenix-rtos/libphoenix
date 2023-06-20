/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * math.h
 *
 * Copyright 2017, 2022 Phoenix Systems
 * Author: Aleksander Kaminski, Damian Loewnau
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_MATH_H_
#define _LIBPHOENIX_MATH_H_


#include <stdarg.h>
#include <math/consts.h>


#ifdef __cplusplus
extern "C" {
#endif

#define FP_NAN       0
#define FP_INFINITE  1
#define FP_ZERO      2
#define FP_SUBNORMAL 3
#define FP_NORMAL    4


#define fpclassify(x)        __builtin_fpclassify(FP_NAN, FP_INFINITE, FP_NORMAL, FP_SUBNORMAL, FP_ZERO, x)
#define isfinite(x)          __builtin_isfinite(x)
#define isgreater(x, y)      __builtin_isgreater(x, y)
#define isgreaterequal(x, y) __builtin_isgreaterequal(x, y)
#define isinf(x)             __builtin_isinf(x)
#define isless(x, y)         __builtin_isless(x, y)
#define islessequal(x, y)    __builtin_islessequal(x, y)
#define islessgreater(x, y)  __builtin_islessgreater(x, y)
#define isnan(x)             __builtin_isnan(x)
#define signbit(x)           __builtin_signbit(x)
#define isnormal(x)          __builtin_isnormal(x)
#define isunordered(x, y)    __builtin_isunordered(x, y)

#define HUGE_VAL  __builtin_huge_val()
#define HUGE_VALF __builtin_huge_valf()
#define HUGE_VALL __builtin_huge_vall()
#define INFINITY  __builtin_inff()
#define NAN       __builtin_nanf("")


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
extern double frexp(double x, int *exp);


/* Returns the result of multiplying x (the significand) by 2 raised to the power of exp (the exponent). */
extern double ldexp(double x, int exp);


/* Returns the natural logarithm of x. */
extern double log(double x);


/* Returns the common (base-2) logarithm of x. */
extern double log2(double x);


/* Returns the common (base-10) logarithm of x. */
extern double log10(double x);


/* Breaks x into an integral and a fractional part. */
extern double modf(double x, double *intpart);
extern float modff(float x, float *intpart);


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
float cosf(float x);
float sinf(float x);
float tanf(float x);
float acosf(float x);
float asinf(float x);
float atanf(float x);
float atan2f(float y, float x);
float coshf(float x);
float sinhf(float x);
float tanhf(float x);
float expf(float x);
float frexpf(float x, int *exp);
float ldexpf(float x, int exp);
float logf(float x);
float log10f(float x);
float powf(float base, float exponent);
float sqrtf(float x);
float roundf(float x);
float ceilf(float x);
float floorf(float x);
float fmodf(float num, float denom);
float truncf(float x);
float fabsf(float x);


#define cosf(x)              ((float)cos(x))
#define sinf(x)              ((float)sin(x))
#define tanf(x)              ((float)tan(x))
#define acosf(x)             ((float)acos(x))
#define asinf(x)             ((float)asin(x))
#define atanf(x)             ((float)atan(x))
#define atan2f(y, x)         ((float)atan2(y, x))
#define coshf(x)             ((float)cosh(x))
#define sinhf(x)             ((float)sinh(x))
#define tanhf(x)             ((float)tanh(x))
#define expf(x)              ((float)exp(x))
#define frexpf(x, exp)       ((float)frexp(x, exp))
#define ldexpf(x, exp)       ((float)ldexp(x, exp))
#define logf(x)              ((float)log(x))
#define log10f(x)            ((float)log10(x))
#define powf(base, exponent) ((float)pow(base, exponent))
#define roundf(x)            ((float)round(x))
#define ceilf(x)             ((float)ceil(x))
#define floorf(x)            ((float)floor(x))
#define fmodf(num, denom)    ((float)fmod(num, denom))
#define truncf(x)            ((float)trunc(x))
#define fabsf(x)             ((float)fabs(x))


#ifdef __cplusplus
}
#endif


#endif
