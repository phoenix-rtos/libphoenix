/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */

/**
 *
 * This family of functions implements the hyperbolic arc cosine of :math:`x`.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     float acoshf(float x);
 *     double acosh(double x);
 *     long double acoshl(long double x);
 *
 * Description
 * ===========
 *
 * ``acosh`` computes the hyperbolic inverse cosine (*hyperbolic arc cosine*)
 * of the input value.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    acosh(x) \approx cosh^{-1}(x) = ln \left( x + \sqrt{x^2-1} \right)
 *
 * Returns
 * =======
 *
 * ``acosh`` returns the hyperbolic inverse cosine, in the range
 * :math:`\mathbb{F}^{+}`.
 *
 * Exceptions
 * ==========
 *
 * Raise ``invalid operation`` exception when the input value is :math:`<1`.
 *
 * Output map
 * ==========
 *
 * +---------------------+----------------------+----------------------+----------------------+----------------------+----------------------+----------------------+
 * | **x**               | :math:`-Inf`         | :math:`<+1`          | :math:`+1`           | :math:`>+1`          | :math:`+Inf`         | :math:`NaN`          |
 * +=====================+======================+======================+======================+======================+======================+======================+
 * | **acosh(x)**        | :math:`qNaN`         | :math:`qNaN`         | :math:`+0`           | :math:`cosh^{-1}(x)` | :math:`+Inf`         | :math:`qNaN`         |
 * +---------------------+----------------------+----------------------+----------------------+----------------------+----------------------+----------------------+
 *
 */

#include <math.h>
#include "../common/tools.h"

#ifndef __LIBMCS_DOUBLE_IS_32BITS

static const double
one    = 1.0,
ln2    = 6.93147180559945286227e-01;  /* 0x3FE62E42, 0xFEFA39EF */

double acosh(double x)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    double t;
    int32_t hx;
    uint32_t lx;
    EXTRACT_WORDS(hx, lx, x);

    if (hx < 0x3ff00000) {     /* x < 1 */
        if (isnan(x)) {
            return x + x;
        } else {
            return __raise_invalid();
        }
    } else if (hx >= 0x41b00000) {  /* x > 2**28 */
        if (hx >= 0x7ff00000) {  /* x is +inf or NaN */
            return x + x;
        } else {
            return log(x) + ln2;    /* acosh(huge)=log(2x) */
        }
    } else if (((hx - 0x3ff00000) | lx) == 0) {
        return 0.0;            /* acosh(1) = 0 */
    } else if (hx > 0x40000000) {    /* 2**28 > x > 2 */
        t = x * x;
        return log(2.0 * x - one / (x + sqrt(t - one)));
    } else {            /* 1<x<2 */
        t = x - one;
        return log1p(t + sqrt(2.0 * t + t * t));
    }
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double acoshl(long double x)
{
    return (long double) acosh((double) x);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
