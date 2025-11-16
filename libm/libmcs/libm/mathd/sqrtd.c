/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */

/**
 *
 * This family of functions implements the square root of :math:`x`.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     float sqrtf(float x);
 *     double sqrt(double x);
 *     long double sqrtl(long double x);
 *
 * Description
 * ===========
 *
 * ``sqrt`` computes the square root of the input value.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    sqrt(x) \approx \sqrt{x}
 *
 * Returns
 * =======
 *
 * ``sqrt`` returns the square root of the input value.
 *
 * Exceptions
 * ==========
 *
 * Raise ``invalid operation`` exception when :math:`x` is negative.
 *
 * Output map
 * ==========
 *
 * +---------------------+--------------+------------------+--------------+--------------+------------------+--------------+--------------+
 * | **x**               | :math:`-Inf` | :math:`<0`       | :math:`-0`   | :math:`+0`   | :math:`>0`       | :math:`+Inf` | :math:`NaN`  |
 * +=====================+==============+==================+==============+==============+==================+==============+==============+
 * | **sqrt(x)**         | :math:`qNaN` | :math:`qNaN`     | :math:`x`                   | :math:`\sqrt{x}` | :math:`+Inf` | :math:`qNaN` |
 * +---------------------+--------------+------------------+--------------+--------------+------------------+--------------+--------------+
 *
 */

#include <math.h>
#include "../common/tools.h"

#ifndef __LIBMCS_DOUBLE_IS_32BITS

double sqrt(double x)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    double z;
    int32_t sign = 0x80000000U;
    uint32_t r, t1, s1, ix1, q1;
    int32_t ix0, s0, q, m, t, i;

    EXTRACT_WORDS(ix0, ix1, x);

    /* take care of Inf and NaN */
    if ((ix0 & 0x7ff00000) == 0x7ff00000) {
        if (isnan(x)) {         /* sqrt(NaN)=NaN */
            return x + x;
        } else if (ix0 > 0) {   /* sqrt(+inf)=+inf */
            return x;
        } else {                /* sqrt(-inf)=sNaN */
            return __raise_invalid();
        }
    }

    /* take care of zero and negative values */
    if (ix0 <= 0) {
        if (((ix0 & (~sign)) | ix1) == 0) {
            return x;    /* sqrt(+-0) = +-0 */
        } else if (ix0 < 0) {
            return __raise_invalid();    /* sqrt(-ve) = sNaN */
        } else {
            /* No action required */
        }
    }

    /* normalize x */
    m = (ix0 >> 20);

    if (m == 0) {             /* subnormal x */
        while (ix0 == 0) {
            m -= 21;
            ix0 |= (ix1 >> 11);
            ix1 <<= 21;
        }

        for (i = 0; (ix0 & 0x00100000) == 0; i++) {
            ix0 <<= 1;
        }

        m -= i - 1;
        ix0 |= (ix1 >> (32 - i));
        ix1 <<= i;
    }

    m -= 1023;    /* unbias exponent */
    ix0 = (ix0 & 0x000fffff) | 0x00100000;

    if (0 < (m & 1)) { /* odd m, double x to make it even */
        ix0 += ix0 + (int32_t)((ix1 & (uint32_t)sign) >> 31U);
        ix1 += ix1;
    }

    m >>= 1;    /* m = [m/2] */

    /* generate sqrt(x) bit by bit */
    ix0 += ix0 + (int32_t)((ix1 & (uint32_t)sign) >> 31U);
    ix1 += ix1;
    q = q1 = s0 = s1 = 0;    /* [q,q1] = sqrt(x) */
    r = 0x00200000;        /* r = moving bit from right to left */

    while (r != 0) {
        t = s0 + r;

        if (t <= ix0) {
            s0   = t + r;
            ix0 -= t;
            q   += r;
        }

        ix0 += ix0 + (int32_t)((ix1 & (uint32_t)sign) >> 31U);
        ix1 += ix1;
        r >>= 1;
    }

    r = sign;

    while (r != 0) {
        t1 = s1 + r;
        t  = s0;

        if ((t < ix0) || ((t == ix0) && (t1 <= ix1))) {
            s1  = t1 + r;

            if ((((int32_t)t1 & sign) == sign) && ((int32_t)s1 & sign) == 0) {
                s0 += 1;
            }

            ix0 -= t;

            if (ix1 < t1) {
                ix0 -= 1;
            }

            ix1 -= t1;
            q1  += r;
        }

        ix0 += ix0 + (int32_t)((ix1 & (uint32_t)sign) >> 31U);
        ix1 += ix1;
        r >>= 1;
    }

    /* use floating add to find out rounding direction */
    if ((ix0 | ix1) != 0) {
        (void) __raise_inexact(x);
        if (q1 == (uint32_t)0xffffffffU) {
            q1 = 0;
            q += 1;
        } else {
            q1 += (q1 & 1);
        }
    }

    ix0 = (q >> 1) + 0x3fe00000;
    ix1 =  q1 >> 1;

    if ((q & 1) != 0) {
        ix1 |= sign;
    }

    ix0 += (m << 20);
    INSERT_WORDS(z, ix0, ix1);
    return z;
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double sqrtl(long double x)
{
    return (long double) sqrt((double) x);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
