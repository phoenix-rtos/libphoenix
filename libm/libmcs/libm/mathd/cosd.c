/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */

/**
 *
 * This family of functions implements the cosine of :math:`x`.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     float cosf(float x);
 *     double cos(double x);
 *     long double cosl(long double x);
 *
 * Description
 * ===========
 *
 * ``cos`` computes the cosine of the input value.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    cos(x) \approx cos(x)
 *
 * Returns
 * =======
 *
 * ``cos`` returns the cosine of the input value, in the range :math:`[-1, 1]`.
 *
 * Exceptions
 * ==========
 *
 * Raise ``invalid operation`` exception when the input value is infinite.
 *
 * Output map
 * ==========
 *
 * +---------------------+----------------+----------------+----------------+----------------+----------------+----------------+----------------+
 * | **x**               | :math:`-Inf`   | :math:`<0`     | :math:`-0`     | :math:`+0`     | :math:`>0`     | :math:`+Inf`   | :math:`NaN`    |
 * +=====================+================+================+================+================+================+================+================+
 * | **cos(x)**          | :math:`qNaN`   | :math:`cos(x)` | :math:`1`                       | :math:`cos(x)` | :math:`qNaN`   | :math:`qNaN`   |
 * +---------------------+----------------+----------------+----------------+----------------+----------------+----------------+----------------+
 *
 */

#include <math.h>
#include "../common/tools.h"
#include "internal/trigd.h"

#ifndef __LIBMCS_DOUBLE_IS_32BITS

double cos(double x)
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

    if(ix <= 0x3fe921fb) {
        if(ix < 0x3e46a09e) {        /* if x < 2**-27 * sqrt(2) */
            if (x == 0.0) {          /* return 1 inexact except 0 */
                return 1.0;
            } else {
                return __raise_inexactf(1.0);
            }
        }

        return __cos(x, z);
    }

    /* cos(Inf or NaN) is NaN */
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
            return  __cos(y[0], y[1]);

        case 1:
            return -__sin(y[0], y[1], 1);

        case 2:
            return -__cos(y[0], y[1]);

        default:
            return  __sin(y[0], y[1], 1);
        }
    }
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double cosl(long double x)
{
    return (long double) cos((double) x);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
