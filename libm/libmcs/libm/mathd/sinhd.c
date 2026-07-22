/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */

/**
 *
 * This family of functions implements the hyperbolic sine of :math:`x`.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     float sinhf(float x);
 *     double sinh(double x);
 *     long double sinhl(long double x);
 *
 * Description
 * ===========
 *
 * ``sinh`` computes the hyperbolic sine of the input value.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    sinh(x) \approx sinh(x) = \frac{e^x-e^{-x}}{2}
 *
 * Returns
 * =======
 *
 * ``sinh`` returns the hyperbolic sine.
 *
 * Exceptions
 * ==========
 *
 * Raise ``overflow`` exception when the magnitude of the input value is too
 * large.
 *
 * .. May raise ``underflow`` exception.
 *
 * Output map
 * ==========
 *
 * +---------------------+--------------+-----------------+--------------+--------------+-----------------+--------------+--------------+
 * | **x**               | :math:`-Inf` | :math:`<0`      | :math:`-0`   | :math:`+0`   | :math:`>0`      | :math:`+Inf` | :math:`NaN`  |
 * +=====================+==============+=================+==============+==============+=================+==============+==============+
 * | **sinh(x)**         | :math:`-Inf` | :math:`sinh(x)` | :math:`x`                   | :math:`sinh(x)` | :math:`+Inf` | :math:`qNaN` |
 * +---------------------+--------------+-----------------+--------------+--------------+-----------------+--------------+--------------+
 *
 */

#include <math.h>
#include "../common/tools.h"

#ifndef __LIBMCS_DOUBLE_IS_32BITS

static const double one = 1.0;

double sinh(double x)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    double t, w, h;
    int32_t ix, jx;
    uint32_t lx;

    /* High word of |x|. */
    GET_HIGH_WORD(jx, x);
    ix = jx & 0x7fffffff;

    /* x is INF or NaN */
    if (ix >= 0x7ff00000) {
        return x + x;
    }

    h = 0.5;

    if (jx < 0) {
        h = -h;
    }

    /* |x| in [0,22], return sign(x)*0.5*(E+E/(E+1))) */
    if (ix < 0x40360000) {          /* |x|<22 */
        if (ix < 0x3e300000) {      /* |x|<2**-28 */
            if (x == 0.0) {         /* return x inexact except 0 */
                return x;
            } else {
                return __raise_inexact(x);
            }
        }

        t = expm1(fabs(x));

        if (ix < 0x3ff00000) {
            return h * (2.0 * t - t * t / (t + one));
        }

        return h * (t + t / (t + one));
    }

    /* |x| in [22, log(maxdouble)] return 0.5*exp(|x|) */
    if (ix < 0x40862E42) {
        return h * exp(fabs(x));
    }

    /* |x| in [log(maxdouble), overflowthresold] */
    GET_LOW_WORD(lx, x);

    if (ix < 0x408633CE || (ix == 0x408633ce && lx <= (uint32_t)0x8fb9f87dU)) {
        w = exp(0.5 * fabs(x));
        t = h * w;
        return t * w;
    }

    /* |x| > overflowthresold, sinh(x) overflow */
    return __raise_overflow(x);
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double sinhl(long double x)
{
    return (long double) sinh((double) x);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
