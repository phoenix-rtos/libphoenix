/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */

/**
 *
 * This family of functions implements the hyperbolic arc tangent of :math:`x`.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     float atanhf(float x);
 *     double atanh(double x);
 *     long double atanhl(long double x);
 *
 * Description
 * ===========
 *
 * ``atanh`` computes the hyperbolic inverse tangent (*hyperbolic arc tangent*)
 * of the input value.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    atanh(x) \approx tanh^{-1}(x) = \frac{1}{2} ln \left( \frac{1+x}{1-x} \right)
 *
 * Returns
 * =======
 *
 * ``atanh`` returns the hyperbolic inverse tangent.
 *
 * Exceptions
 * ==========
 *
 * Raise ``invalid operation`` exception when the input value is not in the
 * interval :math:`[-1, 1]`.
 *
 * Raise ``divide by zero`` exception when the input value is :math:`-1` or
 * :math:`+1`.
 *
 * .. May raise ``underflow`` exception.
 *
 * Output map
 * ==========
 *
 * +---------------------+--------------+--------------+--------------+---------------------+--------------+--------------+---------------------+--------------+--------------+--------------+--------------+
 * | **x**               | :math:`-Inf` | :math:`<-1`  | :math:`-1`   | :math:`\in ]-1,-0[` | :math:`-0`   | :math:`+0`   | :math:`\in ]+0,+1[` | :math:`+1`   | :math:`>+1`  | :math:`+Inf` | :math:`NaN`  |
 * +=====================+==============+==============+==============+=====================+==============+==============+=====================+==============+==============+==============+==============+
 * | **atanh(x)**        | :math:`qNaN` | :math:`qNaN` | :math:`-Inf` | :math:`tanh^{-1} x` | :math:`x`                   | :math:`tanh^{-1} x` | :math:`+Inf` | :math:`qNaN` | :math:`qNaN` | :math:`qNaN` |
 * +---------------------+--------------+--------------+--------------+---------------------+--------------+--------------+---------------------+--------------+--------------+--------------+--------------+
 *
 */

#include <math.h>
#include "../common/tools.h"

#ifndef __LIBMCS_DOUBLE_IS_32BITS

static const double one = 1.0;

double atanh(double x)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    double t;
    int32_t hx, ix;
    uint32_t lx;
    EXTRACT_WORDS(hx, lx, x);
    ix = hx & 0x7fffffff;

    if ((ix | ((lx | (-lx)) >> 31)) > 0x3ff00000) { /* |x|>1 */
        if (isnan(x)) {
            return x + x;
        } else {
            return __raise_invalid();
        }
    }

    if (ix == 0x3ff00000) {
        return __raise_div_by_zero(x);
    }

    if (ix < 0x3e300000) {     /* x<2**-28 */
        if (x == 0.0) {        /* return x inexact except 0 */
            return x;
        } else {
            return __raise_inexact(x);
        }
    }

    SET_HIGH_WORD(x, ix);

    if (ix < 0x3fe00000) {     /* x < 0.5 */
        t = x + x;
        t = 0.5 * log1p(t + t * x / (one - x));
    } else {
        t = 0.5 * log1p((x + x) / (one - x));
    }

    if (hx >= 0) {
        return t;
    } else {
        return -t;
    }
}
#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double atanhl(long double x)
{
    return (long double) atanh((double) x);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
