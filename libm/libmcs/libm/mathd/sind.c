/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */

/**
 *
 * This family of functions implements the sine of :math:`x`.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     float sinf(float x);
 *     double sin(double x);
 *     long double sinl(long double x);
 *
 * Description
 * ===========
 *
 * ``sin`` computes the sine of the input value.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    sin(x) \approx sin(x)
 *
 * Returns
 * =======
 *
 * ``sin`` returns the sine of the input value, in the range :math:`[-1, 1]`.
 *
 * Exceptions
 * ==========
 *
 * Raise ``invalid operation`` exception when the input value is infinite.
 *
 * .. May raise ``underflow`` exception.
 *
 * Output map
 * ==========
 *
 * +---------------------+----------------+----------------+----------------+----------------+----------------+----------------+----------------+
 * | **x**               | :math:`-Inf`   | :math:`<0`     | :math:`-0`     | :math:`+0`     | :math:`>0`     | :math:`+Inf`   | :math:`NaN`    |
 * +=====================+================+================+================+================+================+================+================+
 * | **sin(x)**          | :math:`qNaN`   | :math:`sin(x)` | :math:`x`                       | :math:`sin(x)` | :math:`qNaN`   | :math:`qNaN`   |
 * +---------------------+----------------+----------------+----------------+----------------+----------------+----------------+----------------+
 *
 */

#include <math.h>
#include "../common/tools.h"
#include "internal/trigd.h"

#ifndef __LIBMCS_DOUBLE_IS_32BITS

double sin(double x)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    double y[2], z = 0.0;
    int32_t n, ix;

    /* High word of x. */
    GET_HIGH_WORD(ix, x);

    /* |x| ~< pi/4 */
    ix &= 0x7fffffff;

    if (ix <= 0x3fe921fb) {
        if(ix < 0x3e500000) {      /* |x| < 2**-26 */
            if (x == 0.0) {        /* return x inexact except 0 */
                return x;
            } else {
                return __raise_inexact(x);
            }
        }

        return __sin(x, z, 0);
    }

    /* sin(Inf or NaN) is NaN */
    else if (ix >= 0x7ff00000) {
        if (isnan(x)) {
            return x + x;
        } else {
            return __raise_invalid();
        }
    }

    /* argument reduction needed */
    else {
        n = __rem_pio2(x, y);

        switch (n & 3) {
        case 0:
            return  __sin(y[0], y[1], 1);

        case 1:
            return  __cos(y[0], y[1]);

        case 2:
            return -__sin(y[0], y[1], 1);

        default:
            return -__cos(y[0], y[1]);
        }
    }
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double sinl(long double x)
{
    return (long double) sin((double) x);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
