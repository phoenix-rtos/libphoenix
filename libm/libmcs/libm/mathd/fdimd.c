/* SPDX-License-Identifier: RedHat */
/* Copyright (C) 2002 by  Red Hat, Incorporated. All rights reserved. */

/**
 *
 * This family of functions implements the positive difference value of
 * :math:`x` subtracted by :math:`y`.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     float fdimf(float x, float y);
 *     double fdim(double x, double y);
 *     long double fdiml(long double x, long double y);
 *
 * Description
 * ===========
 *
 * ``fdim`` computes the positive difference value of :math:`x` subtracted by
 * :math:`y`.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    fdim(x, y) \approx \left\{\begin{array}{ll} x - y, & x > y \\ 0, & otherwise \end{array}\right.
 *
 * Returns
 * =======
 *
 * ``fdim`` returns the positive difference value of :math:`x` subtracted by
 * :math:`y`.
 *
 * Exceptions
 * ==========
 *
 * Raise ``overflow`` exception when the result overflows.
 *
 * .. May raise ``underflow`` exception.
 *
 * Output map
 * ==========
 *
 * +--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+
 * | fdim(x,y)                | x                                                                                                                                                                                          |
 * +--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+
 * | y                        | :math:`-Inf`             | :math:`<0`               | :math:`-0`               | :math:`+0`               | :math:`>0`               | :math:`+Inf`             | :math:`NaN`              |
 * +==========================+==========================+==========================+==========================+==========================+==========================+==========================+==========================+
 * | :math:`-Inf`             | :math:`+0`               | :math:`+Inf`                                                                                                                         | :math:`qNaN`             |
 * +--------------------------+                          +--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+
 * | :math:`<0`               |                          | :math:`fdim(x, y)`       | :math:`x - y`                                       | :math:`x - y`            | :math:`+Inf`             |                          |
 * +--------------------------+                          +--------------------------+--------------------------+--------------------------+                          +                          +                          +
 * | :math:`-0`               |                          | :math:`+0`               | :math:`+0`                                          |                          |                          |                          |
 * +--------------------------+                          +                          +                                                     +                          +                          +                          +
 * | :math:`+0`               |                          |                          |                                                     |                          |                          |                          |
 * +--------------------------+                          +                          +                                                     +--------------------------+                          +                          +
 * | :math:`>0`               |                          |                          |                                                     | :math:`fdim(x, y)`       |                          |                          |
 * +--------------------------+                          +--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+                          +
 * | :math:`+Inf`             |                          | :math:`+0`                                                                                                                           |                          |
 * +--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+                          +
 * | :math:`NaN`              | :math:`qNaN`                                                                                                                                                    |                          |
 * +--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+
 *
 */

#include <math.h>

#ifndef __LIBMCS_DOUBLE_IS_32BITS

double fdim(double x, double y)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_one;
    y *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    if (isnan(x) || isnan(y)) {
        return x * y;
    }

    return x > y ? x - y : 0.0;
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double fdiml(long double x, long double y)
{
    return (long double) fdim((double) x, (double) y);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
