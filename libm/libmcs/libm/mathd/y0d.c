/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */

/**
 *
 * This family of functions implements the Bessel function of the second kind
 * of order 0.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     double y0(double x);
 *
 * Description
 * ===========
 *
 * ``y0`` computes the Bessel value of :math:`x` of the second kind of order 0.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    y0(x) = Y_{0}(x)
 *
 * Notice that the mathematical function represented by the procedure ``y0`` is
 * not :math:`y_0` (which is the spherical version of the Bessel function) but
 * :math:`Y_0`. See `WolframAlpha Y_0(x)
 * <https://www.wolframalpha.com/input/?i=Y_0%28x%29>`_ for what it looks like
 * and `Wikipedia <https://en.wikipedia.org/wiki/Bessel_function>`_ for more
 * information.
 *
 * Returns
 * =======
 *
 * ``y0`` returns the Bessel value of :math:`x` of the second kind of order 0.
 *
 * Exceptions
 * ==========
 *
 * Raise ``invalid operation`` exception when the input value is negative.
 *
 * Raise ``divide by zero`` exception when the input value is zero.
 *
 * .. May raise ``underflow`` exception.
 *
 * Output map
 * ==========
 *
 * +---------------------+--------------+------------------+--------------+--------------+------------------+--------------+--------------+
 * | **x**               | :math:`-Inf` | :math:`<0`       | :math:`-0`   | :math:`+0`   | :math:`>0`       | :math:`+Inf` | :math:`NaN`  |
 * +=====================+==============+==================+==============+==============+==================+==============+==============+
 * | **y0(x)**           | :math:`qNaN`                    | :math:`-Inf`                | :math:`Y_{0}(x)` | :math:`+0`   | :math:`qNaN` |
 * +---------------------+--------------+------------------+--------------+--------------+------------------+--------------+--------------+
 *
 */

#include <math.h>
#include "internal/besseld.h"
#include "../common/tools.h"

#ifndef __LIBMCS_DOUBLE_IS_32BITS

static const double
u00  = -7.38042951086872317523e-02, /* 0xBFB2E4D6, 0x99CBD01F */
u01  =  1.76666452509181115538e-01, /* 0x3FC69D01, 0x9DE9E3FC */
u02  = -1.38185671945596898896e-02, /* 0xBF8C4CE8, 0xB16CFA97 */
u03  =  3.47453432093683650238e-04, /* 0x3F36C54D, 0x20B29B6B */
u04  = -3.81407053724364161125e-06, /* 0xBECFFEA7, 0x73D25CAD */
u05  =  1.95590137035022920206e-08, /* 0x3E550057, 0x3B4EABD4 */
u06  = -3.98205194132103398453e-11, /* 0xBDC5E43D, 0x693FB3C8 */
v01  =  1.27304834834123699328e-02, /* 0x3F8A1270, 0x91C9C71A */
v02  =  7.60068627350353253702e-05, /* 0x3F13ECBB, 0xF578C6C1 */
v03  =  2.59150851840457805467e-07, /* 0x3E91642D, 0x7FF202FD */
v04  =  4.41110311332675467403e-10; /* 0x3DFE5018, 0x3BD6D9EF */

double y0(double x)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    double z, s, c, ss, cc, u, v;
    int32_t hx, ix, lx;

    EXTRACT_WORDS(hx, lx, x);
    ix = 0x7fffffff & hx;

    if (ix >= 0x7ff00000) {
        if (isnan(x)) {     /* y0(NaN) = NaN */
            return x + x;
        } else if (hx > 0) {  /* y0(+Inf) = +0.0 */
            return zero;
        } else {
            /* No action required */
        }
    }

    if ((ix | lx) == 0) {   /* y0(+-0) = +Inf */
        return __raise_div_by_zero(-1.0);
    }

    if (hx < 0) {           /* y0(<0) = NaN, y0(-Inf) = NaN */
        return __raise_invalid();
    }

    if (ix >= 0x40000000) { /* |x| >= 2.0 */
        /* y0(x) = sqrt(2/(pi*x))*(p0(x)*sin(x0)+q0(x)*cos(x0))
         * where x0 = x-pi/4
         *      Better formula:
         *              cos(x0) = cos(x)cos(pi/4)+sin(x)sin(pi/4)
         *                      =  1/sqrt(2) * (sin(x) + cos(x))
         *              sin(x0) = sin(x)cos(3pi/4)-cos(x)sin(3pi/4)
         *                      =  1/sqrt(2) * (sin(x) - cos(x))
         * To avoid cancellation, use
         *              sin(x) +- cos(x) = -cos(2x)/(sin(x) -+ cos(x))
         * to compute the worse one.
         */
        s = sin(x);
        c = cos(x);
        ss = s - c;
        cc = s + c;

        /*
         * j0(x) = 1/sqrt(pi) * (P(0,x)*cc - Q(0,x)*ss) / sqrt(x)
         * y0(x) = 1/sqrt(pi) * (P(0,x)*ss + Q(0,x)*cc) / sqrt(x)
         */
        if (ix < 0x7fe00000) { /* make sure x+x not overflow */
            z = -cos(x + x);

            if ((s * c) < zero) {
                cc = z / ss;
            } else {
                ss = z / cc;
            }
        }

        if (ix > 0x48000000) {
            z = (invsqrtpi * ss) / sqrt(x);
        } else {
            u = __j0_p(x);
            v = __j0_q(x);
            z = invsqrtpi * (u * ss + v * cc) / sqrt(x);
        }

        return z;
    }

    if (ix <= 0x3e400000) { /* x < 2**-27 */
        return (u00 + tpi * log(x));
    }

    z = x * x;
    u = u00 + z * (u01 + z * (u02 + z * (u03 + z * (u04 + z * (u05 + z * u06)))));
    v = one + z * (v01 + z * (v02 + z * (v03 + z * v04)));
    return (u / v + tpi * (j0(x) * log(x)));
}

#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
