/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */

/**
 *
 * This family of functions implements the hyperbolic tangent of :math:`x`.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     float tanhf(float x);
 *     double tanh(double x);
 *     long double tanhl(long double x);
 *
 * Description
 * ===========
 *
 * ``tanh`` computes the hyperbolic tangent of the input value.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    tanh(x) \approx tanh(x) = \frac{e^x-e^{-x}}{e^x+e^{-x}}
 *
 * Returns
 * =======
 *
 * ``tanh`` returns the hyperbolic tangent, in the range :math:`[-1, 1]`.
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
 * +---------------------+--------------+-----------------+--------------+--------------+-----------------+--------------+--------------+
 * | **x**               | :math:`-Inf` | :math:`<0`      | :math:`-0`   | :math:`+0`   | :math:`>0`      | :math:`+Inf` | :math:`NaN`  |
 * +=====================+==============+=================+==============+==============+=================+==============+==============+
 * | **tanh(x)**         | :math:`-1`   | :math:`tanh(x)` | :math:`x`                   | :math:`tanh(x)` | :math:`+1`   | :math:`qNaN` |
 * +---------------------+--------------+-----------------+--------------+--------------+-----------------+--------------+--------------+
 *
 */

#include <math.h>
#include "../common/tools.h"

#ifndef __LIBMCS_DOUBLE_IS_32BITS

static const double one = 1.0, two = 2.0;

double tanh(double x)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    double t, z;
    int32_t jx, ix;

    /* High word of |x|. */
    GET_HIGH_WORD(jx, x);
    ix = jx & 0x7fffffff;

    /* x is INF or NaN */
    if (ix >= 0x7ff00000) {
        if (isnan(x)) {             /* tanh(NaN) = NaN */
            return x + x;
        } else if (jx >= 0) {
            return one;             /* tanh(+inf)=+1 */
        } else {
            return -one;            /* tanh(-inf)=-1 */
        }
    }

    /* |x| < 22 */
    if (ix < 0x40360000) {          /* |x|<22 */
        if (ix < 0x3c800000) {      /* |x|<2**-55 */
            if (x == 0.0) {         /* return x inexact except 0 */
                return x;
            } else {
                return __raise_inexact(x);
            }
        }

        if (ix >= 0x3ff00000) {     /* |x|>=1  */
            t = expm1(two * fabs(x));
            z = one - two / (t + two);
        } else {
            t = expm1(-two * fabs(x));
            z = -t / (t + two);
        }

    /* |x| > 22, return +-1 */
    } else {
        z = __raise_inexact(one);               /* raised inexact flag */
    }

    return (jx >= 0) ? z : -z;
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double tanhl(long double x)
{
    return (long double) tanh((double) x);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
