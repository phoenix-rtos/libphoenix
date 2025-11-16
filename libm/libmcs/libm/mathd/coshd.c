/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */

/**
 *
 * This family of functions implements the hyperbolic cosine of :math:`x`.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     float coshf(float x);
 *     double cosh(double x);
 *     long double coshl(long double x);
 *
 * Description
 * ===========
 *
 * ``cosh`` computes the hyperbolic cosine of the input value.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    cosh(x) \approx cosh(x) = \frac{e^x+e^{-x}}{2}
 *
 * Returns
 * =======
 *
 * ``cosh`` returns the hyperbolic cosine, in the range :math:`\mathbb{F}_{>=1}`.
 *
 * Exceptions
 * ==========
 *
 * Raise ``overflow`` exception when the magnitude of the input value is too large.
 *
 * Output map
 * ==========
 *
 * +---------------------+--------------+-----------------+--------------+--------------+-----------------+--------------+--------------+
 * | **x**               | :math:`-Inf` | :math:`<0`      | :math:`-0`   | :math:`+0`   | :math:`>0`      | :math:`+Inf` | :math:`NaN`  |
 * +=====================+==============+=================+==============+==============+=================+==============+==============+
 * | **cosh(x)**         | :math:`+Inf` | :math:`cosh(x)` | :math:`1`                   | :math:`cosh(x)` | :math:`+Inf` | :math:`qNaN` |
 * +---------------------+--------------+-----------------+--------------+--------------+-----------------+--------------+--------------+
 *
 */

#include <math.h>
#include "../common/tools.h"

#ifndef __LIBMCS_DOUBLE_IS_32BITS

static const double one = 1.0, half = 0.5;

double cosh(double x)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    double t, w;
    int32_t ix;
    uint32_t lx;

    /* High word of |x|. */
    GET_HIGH_WORD(ix, x);
    ix &= 0x7fffffff;

    /* x is INF or NaN */
    if (ix >= 0x7ff00000) {
        return x * x;
    }

    /* |x| in [0,0.5*ln2], return 1+expm1(|x|)^2/(2*exp(|x|)) */
    if (ix < 0x3fd62e43) {
        t = expm1(fabs(x));
        w = one + t;

        if (ix < 0x3c800000) {
            return w;    /* cosh(tiny) = 1 */
        }

        return one + (t * t) / (w + w);
    }

    /* |x| in [0.5*ln2,22], return (exp(|x|)+1/exp(|x|)/2; */
    if (ix < 0x40360000) {
        t = exp(fabs(x));
        return half * t + half / t;
    }

    /* |x| in [22, log(maxdouble)] return half*exp(|x|) */
    if (ix < 0x40862E42) {
        return half * exp(fabs(x));
    }

    /* |x| in [log(maxdouble), overflowthresold] */
    GET_LOW_WORD(lx, x);

    if (ix < 0x408633CE ||
        (ix == 0x408633ce && lx <= (uint32_t)0x8fb9f87dU)) {
        w = exp(half * fabs(x));
        t = half * w;
        return t * w;
    }

    /* |x| > overflowthresold, cosh(x) overflow */
    return __raise_overflow(1.0);
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double coshl(long double x)
{
    return (long double) cosh((double) x);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
