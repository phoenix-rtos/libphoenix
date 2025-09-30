/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */

/**
 *
 * This family of functions implements the hypotenuse of a right-angled triangle.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     float hypotf(float x, float y);
 *     double hypot(double x, double y);
 *     long double hypotl(long double x, long double y);
 *
 * Description
 * ===========
 *
 * ``hypot`` computes the length of the hypotenuse of a right-angled triangle
 * where the legs have the lengths :math:`x` and :math:`y`.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    hypot(x, y) \approx \sqrt{x^2 + y^2}
 *
 * Returns
 * =======
 *
 * ``hypot`` returns the length of the hypotenuse of a right-angled triangle.
 *
 * Exceptions
 * ==========
 *
 * Raise ``overflow`` exception when the magnitude of the input values is too
 * large.
 *
 * .. May raise ``underflow`` exception.
 *
 * Output map
 * ==========
 *
 * +--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+
 * | hypot(x,y)               | x                                                                                                         |
 * +--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+
 * | y                        | :math:`-Inf`             | :math:`\in \mathbb{F}`   | :math:`+Inf`             | :math:`NaN`              |
 * +==========================+==========================+==========================+==========================+==========================+
 * | :math:`-Inf`             | :math:`+Inf`             | :math:`+Inf`             | :math:`+Inf`             | :math:`+Inf`             |
 * +--------------------------+                          +--------------------------+                          +--------------------------+
 * | :math:`\in \mathbb{F}`   |                          | :math:`\sqrt{x^2 + y^2}` |                          | :math:`qNaN`             |
 * +--------------------------+                          +--------------------------+                          +--------------------------+
 * | :math:`+Inf`             |                          | :math:`+Inf`             |                          | :math:`+Inf`             |
 * +--------------------------+                          +--------------------------+                          +--------------------------+
 * | :math:`NaN`              |                          | :math:`qNaN`             |                          | :math:`qNaN`             |
 * +--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+
 *
 */

#include <math.h>
#include "../common/tools.h"

#ifndef __LIBMCS_DOUBLE_IS_32BITS

double hypot(double x, double y)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_one;
    y *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    double a = x;
    double b = y;
    double t1, t2;
    double _y1, _y2;
    double w;
    int32_t j, k, ha, hb;

    GET_HIGH_WORD(ha, x);
    ha &= 0x7fffffff;
    GET_HIGH_WORD(hb, y);
    hb &= 0x7fffffff;

    if (hb > ha) {
        a = y;
        b = x;
        j = ha;
        ha = hb;
        hb = j;
    } else {
        a = x;
        b = y;
    }

    SET_HIGH_WORD(a, ha);         /* a <- |a| */
    SET_HIGH_WORD(b, hb);         /* b <- |b| */

    if ((ha - hb) > 0x3c00000) {
        return a + b;             /* x/y > 2**60 */
    }

    k = 0;

    if (ha > 0x5f300000) {        /* a>2**500 */
        if (ha >= 0x7ff00000) {   /* Inf or NaN */
            uint32_t low;
            w = a + b;            /* for sNaN */
            GET_LOW_WORD(low, a);

            if (((ha & 0xfffff) | low) == 0) {
                w = a;
            }

            GET_LOW_WORD(low, b);

            if (((hb ^ 0x7ff00000) | low) == 0) {
                w = b;
            }

            return w;
        }

        /* scale a and b by 2**-600 */
        ha -= 0x25800000;
        hb -= 0x25800000;
        k += 600;
        SET_HIGH_WORD(a, ha);
        SET_HIGH_WORD(b, hb);
    }

    if (hb < 0x20b00000) {        /* b < 2**-500 */
        if (hb <= 0x000fffff) {   /* subnormal b or 0 */
            uint32_t low;
            GET_LOW_WORD(low, b);

            if ((hb | low) == 0) {
                return a;
            }

            t1 = 0;
            SET_HIGH_WORD(t1, 0x7fd00000);   /* t1=2^1022 */
            b *= t1;
            a *= t1;
            k -= 1022;
        } else {        /* scale a and b by 2^600 */
            ha += 0x25800000;     /* a *= 2^600 */
            hb += 0x25800000;     /* b *= 2^600 */
            k -= 600;
            SET_HIGH_WORD(a, ha);
            SET_HIGH_WORD(b, hb);
        }
    }

    /* medium size a and b */
    w = a - b;

    if (w > b) {
        t1 = 0;
        SET_HIGH_WORD(t1, ha);
        t2 = a - t1;
        w  = sqrt(t1 * t1 - (b * (-b) - t2 * (a + t1)));
    } else {
        a  = a + a;
        _y1 = 0;
        SET_HIGH_WORD(_y1, hb);
        _y2 = b - _y1;
        t1 = 0;
        SET_HIGH_WORD(t1, ha + 0x00100000);
        t2 = a - t1;
        w  = sqrt(t1 * _y1 - (w * (-w) - (t1 * _y2 + t2 * b)));
    }

    if (k != 0) {
        t1 = 0.0;
        SET_HIGH_WORD(t1, (0x3FF + k) << 20);
        return t1 * w;
    } else {
        return w;
    }
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double hypotl(long double x, long double y)
{
    return (long double) hypot((double) x, (double) y);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
