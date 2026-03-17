/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */

/**
 *
 * This function implements the Bessel function of the first kind of
 * order 0.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     double j0(double x);
 *
 * Description
 * ===========
 *
 * ``j0`` computes the Bessel value of :math:`x` of the first kind of order 0.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    j0(x) = J_{0}(x)
 *
 * Notice that the mathematical function represented by the procedure ``j0`` is
 * not :math:`j_0` (which is the spherical version of the Bessel function) but
 * :math:`J_0`. See `WolframAlpha
 * <https://www.wolframalpha.com/input/?i=J0%28x%29>`_ for what it looks like
 * and `Wikipedia <https://en.wikipedia.org/wiki/Bessel_function>`_ for more
 * information.
 *
 * Returns
 * =======
 *
 * ``j0`` returns the Bessel value of :math:`x` of the first kind of order 0.
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
 * | **j0(x)**           | :math:`+0`   | :math:`J_{0}(x)`                                          | :math:`+0`   | :math:`qNaN` |
 * +---------------------+--------------+--------------+--------------+--------------+--------------+--------------+--------------+
 *
 */

#include <math.h>
#include "internal/besseld.h"
#include "../common/tools.h"

#ifndef __LIBMCS_DOUBLE_IS_32BITS

static const double
/* R0/S0 on [0, 2.00] */
R02       =  1.56249999999999947958e-02, /* 0x3F8FFFFF, 0xFFFFFFFD */
R03       = -1.89979294238854721751e-04, /* 0xBF28E6A5, 0xB61AC6E9 */
R04       =  1.82954049532700665670e-06, /* 0x3EBEB1D1, 0x0C503919 */
R05       = -4.61832688532103189199e-09, /* 0xBE33D5E7, 0x73D63FCE */
S01       =  1.56191029464890010492e-02, /* 0x3F8FFCE8, 0x82C8C2A4 */
S02       =  1.16926784663337450260e-04, /* 0x3F1EA6D2, 0xDD57DBF4 */
S03       =  5.13546550207318111446e-07, /* 0x3EA13B54, 0xCE84D5A9 */
S04       =  1.16614003333790000205e-09; /* 0x3E1408BC, 0xF4745D8F */

double j0(double x)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    double z, s, c, ss, cc, r, u, v;
    int32_t hx, ix;

    GET_HIGH_WORD(hx, x);
    ix = hx & 0x7fffffff;

    if (ix >= 0x7ff00000) {
        return one / (x * x);
    }

    x = fabs(x);

    if (ix >= 0x40000000) {   /* |x| >= 2.0 */
        s = sin(x);
        c = cos(x);
        ss = s - c;
        cc = s + c;

        if (ix < 0x7fe00000) { /* make sure x+x not overflow */
            z = -cos(x + x);

            if ((s * c) < zero) {
                cc = z / ss;
            } else {
                ss = z / cc;
            }
        }

        /*
         * j0(x) = 1/sqrt(pi) * (P(0,x)*cc - Q(0,x)*ss) / sqrt(x)
         * y0(x) = 1/sqrt(pi) * (P(0,x)*ss + Q(0,x)*cc) / sqrt(x)
         */
        if (ix > 0x48000000) {
            z = (invsqrtpi * cc) / sqrt(x);
        } else {
            u = __j0_p(x);
            v = __j0_q(x);
            z = invsqrtpi * (u * cc - v * ss) / sqrt(x);
        }

        return z;
    }

    if (ix < 0x3f200000) { /* |x| < 2**-13 */
        if (x != 0.0) {
            (void)__raise_inexact(x); /* raise inexact if x != 0 */
        }
        if (ix < 0x3e400000) {
            return one;    /* |x|<2**-27 */
        } else {
            return one - 0.25 * x * x;
        }
    }

    z = x * x;
    r =  z * (R02 + z * (R03 + z * (R04 + z * R05)));
    s =  one + z * (S01 + z * (S02 + z * (S03 + z * S04)));

    if (ix < 0x3FF00000) {   /* |x| < 1.00 */
        return one + z * (-0.25 + (r / s));
    } else {
        u = 0.5 * x;
        return ((one + u) * (one - u) + z * (r / s));
    }
}

#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
