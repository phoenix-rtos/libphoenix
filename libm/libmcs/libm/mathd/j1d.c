/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */

/**
 *
 * This function implements the Bessel function of the first kind of
 * order 1.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     double j1(double x);
 *
 * Description
 * ===========
 *
 * ``j1`` computes the Bessel value of :math:`x` of the first kind of order 1.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    j1(x) = J_{1}(x)
 *
 * Notice that the mathematical function represented by the procedure ``j1`` is
 * not :math:`j_1` (which is the spherical version of the Bessel function) but
 * :math:`J_1`. See `WolframAlpha J1(x)
 * <https://www.wolframalpha.com/input/?i=J1%28x%29>`_ for what it looks like
 * and `Wikipedia <https://en.wikipedia.org/wiki/Bessel_function>`_ for more
 * information.
 *
 * Returns
 * =======
 *
 * ``j1`` returns the Bessel value of :math:`x` of the first kind of order 1.
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
 * +---------------------+--------------+--------------+--------------+--------------+--------------+--------------+--------------+
 * | **x**               | :math:`-Inf` | :math:`<0`   | :math:`-0`   | :math:`+0`   | :math:`>0`   | :math:`+Inf` | :math:`NaN`  |
 * +=====================+==============+==============+==============+==============+==============+==============+==============+
 * | **j1(x)**           | :math:`+0`   | :math:`J_{1}(x)`                                          | :math:`+0`   | :math:`qNaN` |
 * +---------------------+--------------+--------------+--------------+--------------+--------------+--------------+--------------+
 *
 */

#include <math.h>
#include "internal/besseld.h"
#include "../common/tools.h"

#ifndef __LIBMCS_DOUBLE_IS_32BITS

static const double
/* R0/S0 on [0,2] */
r00       = -6.25000000000000000000e-02, /* 0xBFB00000, 0x00000000 */
r01       =  1.40705666955189706048e-03, /* 0x3F570D9F, 0x98472C61 */
r02       = -1.59955631084035597520e-05, /* 0xBEF0C5C6, 0xBA169668 */
r03       =  4.96727999609584448412e-08, /* 0x3E6AAAFA, 0x46CA0BD9 */
s01       =  1.91537599538363460805e-02, /* 0x3F939D0B, 0x12637E53 */
s02       =  1.85946785588630915560e-04, /* 0x3F285F56, 0xB9CDF664 */
s03       =  1.17718464042623683263e-06, /* 0x3EB3BFF8, 0x333F8498 */
s04       =  5.04636257076217042715e-09, /* 0x3E35AC88, 0xC97DFF2C */
s05       =  1.23542274426137913908e-11; /* 0x3DAB2ACF, 0xCFB97ED8 */

double j1(double x)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    double z, s, c, ss, cc, r, u, v, y;
    int32_t hx, ix;

    GET_HIGH_WORD(hx, x);
    ix = hx & 0x7fffffff;

    if (ix >= 0x7ff00000) {
        return one / x;
    }

    y = fabs(x);

    if (ix >= 0x40000000) {   /* |x| >= 2.0 */
        s = sin(y);
        c = cos(y);
        ss = -s - c;
        cc = s - c;

        if (ix < 0x7fe00000) { /* make sure y+y not overflow */
            z = cos(y + y);

            if ((s * c) > zero) {
                cc = z / ss;
            } else {
                ss = z / cc;
            }
        }

        /*
         * j1(x) = 1/sqrt(pi) * (P(1,x)*cc - Q(1,x)*ss) / sqrt(x)
         * y1(x) = 1/sqrt(pi) * (P(1,x)*ss + Q(1,x)*cc) / sqrt(x)
         */
        if (ix > 0x48000000) {
            z = (invsqrtpi * cc) / sqrt(y);
        } else {
            u = __j1_p(y);
            v = __j1_q(y);
            z = invsqrtpi * (u * cc - v * ss) / sqrt(y);
        }

        if (hx < 0) {
            return -z;
        } else {
            return  z;
        }
    }

    if (ix < 0x3e400000) { /* |x|<2**-27 */
        if (x != 0.0) {
            (void)__raise_inexact(x); /* raise inexact if x != 0 */
        }
        return 0.5 * x;
    }

    z = x * x;
    r =  z * (r00 + z * (r01 + z * (r02 + z * r03)));
    s =  one + z * (s01 + z * (s02 + z * (s03 + z * (s04 + z * s05))));
    r *= x;
    return (x * 0.5 + r / s);
}

#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
