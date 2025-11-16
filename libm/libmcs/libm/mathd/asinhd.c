/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */

/**
 *
 * This family of functions implements the hyperbolic arc sine of :math:`x`.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     float asinhf(float x);
 *     double asinh(double x);
 *     long double asinhl(long double x);
 *
 * Description
 * ===========
 *
 * ``asinh`` computes the hyperbolic inverse sine (*hyperbolic arc sine*) of
 * the input value.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    asinh(x) \approx sinh^{-1}(x) = ln \left( x + \sqrt{x^2+1} \right)
 *
 * Returns
 * =======
 *
 * ``asinh`` returns the hyperbolic inverse sine.
 *
 * Exceptions
 * ==========
 *
 * Does not raise overflow, division by zero, and invalid exceptions.
 *
 * .. May raise ``underflow`` exception.
 *
 * Output map
 * ==========
 *
 * +---------------------+----------------------+----------------------+--------------+--------------+----------------------+----------------------+----------------------+
 * | **x**               | :math:`-Inf`         | :math:`<0`           | :math:`-0`   | :math:`+0`   | :math:`>0`           | :math:`+Inf`         | :math:`NaN`          |
 * +=====================+======================+======================+==============+==============+======================+======================+======================+
 * | **asinh(x)**        | :math:`-Inf`         | :math:`sinh^{-1}(x)` | :math:`x`                   | :math:`sinh^{-1}(x)` | :math:`+Inf`         | :math:`qNaN`         |
 * +---------------------+----------------------+----------------------+--------------+--------------+----------------------+----------------------+----------------------+
 *
 */

#include <math.h>
#include "../common/tools.h"

#ifndef __LIBMCS_DOUBLE_IS_32BITS

static const double
one  =  1.00000000000000000000e+00, /* 0x3FF00000, 0x00000000 */
ln2  =  6.93147180559945286227e-01; /* 0x3FE62E42, 0xFEFA39EF */

double asinh(double x)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    double t, w;
    int32_t hx, ix;
    GET_HIGH_WORD(hx, x);
    ix = hx & 0x7fffffff;

    if (ix >= 0x7ff00000) {
        return x + x;    /* x is inf or NaN */
    }

    if (ix < 0x3e300000) {  /* |x|<2**-28 */
        if (x == 0.0) {     /* return x inexact except 0 */
            return x;
        } else {
            return __raise_inexact(x);
        }
    }

    if (ix > 0x41b00000) { /* |x| > 2**28 */
        w = log(fabs(x)) + ln2;
    } else if (ix > 0x40000000) {  /* 2**28 > |x| > 2.0 */
        t = fabs(x);
        w = log(2.0 * t + one / (sqrt(x * x + one) + t));
    } else {        /* 2.0 > |x| > 2**-28 */
        t = x * x;
        w = log1p(fabs(x) + t / (one + sqrt(one + t)));
    }

    if (hx > 0) {
        return w;
    } else {
        return -w;
    }
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double asinhl(long double x)
{
    return (long double) asinh((double) x);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
