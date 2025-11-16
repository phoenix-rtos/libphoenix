/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */

/**
 *
 * This family of functions implements the arc cosine of :math:`x`.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     float acosf(float x);
 *     double acos(double x);
 *     long double acosl(long double x);
 *
 * Description
 * ===========
 *
 * ``acos`` computes the inverse cosine (*arc cosine*) of the input value.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    acos(x) \approx cos^{-1}(x)
 *
 * Returns
 * =======
 *
 * ``acos`` returns value in radians, in the range :math:`[0, \pi]`.
 *
 * Exceptions
 * ==========
 *
 * Raise ``invalid operation`` exception when the input value is not in the
 * interval :math:`[-1, 1]`.
 *
 * .. May raise ``underflow`` exception.
 *
 * Output map
 * ==========
 *
 * +---------------------+--------------+--------------+---------------------+--------------+--------------+--------------+--------------+
 * | **x**               | :math:`-Inf` | :math:`<-1`  | :math:`\in [-1,+1[` | :math:`+1`   | :math:`>+1`  | :math:`+Inf` | :math:`NaN`  |
 * +=====================+==============+==============+=====================+==============+==============+==============+==============+
 * | **acos(x)**         | :math:`qNaN` | :math:`qNaN` | :math:`cos^{-1} x`  | :math:`+0`   | :math:`qNaN` | :math:`qNaN` | :math:`qNaN` |
 * +---------------------+--------------+--------------+---------------------+--------------+--------------+--------------+--------------+
 *
 */

#include <math.h>
#include "../common/tools.h"

#ifndef __LIBMCS_DOUBLE_IS_32BITS

static const double
one     =  1.00000000000000000000e+00, /* 0x3FF00000, 0x00000000 */
pi      =  3.14159265358979311600e+00, /* 0x400921FB, 0x54442D18 */
pio2_hi =  1.57079632679489655800e+00, /* 0x3FF921FB, 0x54442D18 */
pio2_lo =  6.12323399573676603587e-17, /* 0x3C91A626, 0x33145C07 */
pS0     =  1.66666666666666657415e-01, /* 0x3FC55555, 0x55555555 */
pS1     = -3.25565818622400915405e-01, /* 0xBFD4D612, 0x03EB6F7D */
pS2     =  2.01212532134862925881e-01, /* 0x3FC9C155, 0x0E884455 */
pS3     = -4.00555345006794114027e-02, /* 0xBFA48228, 0xB5688F3B */
pS4     =  7.91534994289814532176e-04, /* 0x3F49EFE0, 0x7501B288 */
pS5     =  3.47933107596021167570e-05, /* 0x3F023DE1, 0x0DFDF709 */
qS1     = -2.40339491173441421878e+00, /* 0xC0033A27, 0x1C8A2D4B */
qS2     =  2.02094576023350569471e+00, /* 0x40002AE5, 0x9C598AC8 */
qS3     = -6.88283971605453293030e-01, /* 0xBFE6066C, 0x1B8D0159 */
qS4     =  7.70381505559019352791e-02; /* 0x3FB3B8C5, 0xB12E9282 */

double acos(double x)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    double z, p, q, r, w, s, c, df;
    int32_t hx, ix;
    GET_HIGH_WORD(hx, x);
    ix = hx & 0x7fffffff;

    if (ix >= 0x3ff00000) { /* |x| >= 1 */
        uint32_t lx;
        GET_LOW_WORD(lx, x);

        if (((ix - 0x3ff00000) | lx) == 0) { /* |x|==1 */
            if (hx > 0) {
                return 0.0;    /* acos(1) = 0  */
            } else {
                return __raise_inexact(pi);    /* acos(-1)= pi */
            }
        }

        if (isnan(x)) {
            return x + x;
        }

        return __raise_invalid();  /* acos(|x|>1) is NaN */
    }

    if (ix < 0x3fe00000) { /* |x| < 0.5 */
        if (ix <= 0x3c600000) {
            return __raise_inexact(pio2_hi);    /*if|x|<2**-57*/
        }

        z = x * x;
        p = z * (pS0 + z * (pS1 + z * (pS2 + z * (pS3 + z * (pS4 + z * pS5)))));
        q = one + z * (qS1 + z * (qS2 + z * (qS3 + z * qS4)));
        r = p / q;
        return pio2_hi - (x - (pio2_lo - x * r));
    } else  if (hx < 0) {      /* x < -0.5 */
        z = (one + x) * 0.5;
        p = z * (pS0 + z * (pS1 + z * (pS2 + z * (pS3 + z * (pS4 + z * pS5)))));
        q = one + z * (qS1 + z * (qS2 + z * (qS3 + z * qS4)));
        s = sqrt(z);
        r = p / q;
        w = r * s - pio2_lo;
        return pi - 2.0 * (s + w);
    } else {            /* x > 0.5 */
        z = (one - x) * 0.5;
        s = sqrt(z);
        df = s;
        SET_LOW_WORD(df, 0);
        c  = (z - df * df) / (s + df);
        p = z * (pS0 + z * (pS1 + z * (pS2 + z * (pS3 + z * (pS4 + z * pS5)))));
        q = one + z * (qS1 + z * (qS2 + z * (qS3 + z * qS4)));
        r = p / q;
        w = r * s + c;
        return 2.0 * (df + w);
    }
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double acosl(long double x)
{
    return (long double) acos((double) x);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
