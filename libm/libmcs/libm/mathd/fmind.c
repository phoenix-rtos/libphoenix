/* SPDX-License-Identifier: RedHat */
/* Copyright (C) 2002 by  Red Hat, Incorporated. All rights reserved. */

/**
 *
 * This family of functions determines the minimum numerical value of :math:`x` and :math:`y`.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     float fminf(float x, float y);
 *     double fmin(double x, double y);
 *     long double fminl(long double x, long double y);
 *
 * Description
 * ===========
 *
 * ``fmin`` computes the minimum numerical value of :math:`x` and :math:`y`.
 *
 * Mathematical Function
 * =====================
 * 
 * .. math::
 *
 *    fmin(x, y) = \left\{\begin{array}{ll} x, & x < y \\ y, & otherwise \end{array}\right.
 *
 * Returns
 * =======
 *
 * ``fmin`` returns the minimum numerical value of :math:`x` and :math:`y`.
 *
 * Exceptions
 * ==========
 *
 * Does raise invalid exception in case one of the parameters is :math:`sNaN`.
 *
 * Output map
 * ==========
 *
 * +--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+
 * | fmin(x,y)                | x                                                                                                                                                                                                                     |
 * +--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+
 * | y                        | :math:`-Inf`             | :math:`<0`               | :math:`-0`               | :math:`+0`               | :math:`>0`               | :math:`+Inf`             | :math:`qNaN`             | :math:`sNaN`             |
 * +==========================+==========================+==========================+==========================+==========================+==========================+==========================+==========================+==========================+
 * | :math:`-Inf`             | :math:`y`                                           | :math:`y`                                                                      | :math:`y`                | :math:`y`                | :math:`qNaN`             |
 * +--------------------------+--------------------------+--------------------------+                                                                                +                          +                          +                          +
 * | :math:`<0`               | :math:`x`                | :math:`fmin(x, y)`       |                                                                                |                          |                          |                          |
 * +--------------------------+                          +--------------------------+                                                                                +                          +                          +                          +
 * | :math:`-0`               |                          | :math:`x`                |                                                                                |                          |                          |                          |
 * +--------------------------+                          +                          +                                                                                +                          +                          +                          +
 * | :math:`+0`               |                          |                          |                                                                                |                          |                          |                          |
 * +--------------------------+                          +                          +--------------------------+--------------------------+--------------------------+                          +                          +                          +
 * | :math:`>0`               |                          |                          | :math:`x`                                           | :math:`fmin(x, y)`       |                          |                          |                          |
 * +--------------------------+                          +                          +                                                     +--------------------------+                          +                          +                          +
 * | :math:`+Inf`             |                          |                          |                                                     | :math:`x`                |                          |                          |                          |
 * +--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+                          +                          +
 * | :math:`qNaN`             | :math:`x`                                                                                                                                                       |                          |                          |
 * +--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+                          +
 * | :math:`sNaN`             | :math:`qNaN`                                                                                                                                                                               |                          |
 * +--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+
 *
 */

#include <math.h>
#include "../common/tools.h"

#ifndef __LIBMCS_DOUBLE_IS_32BITS

double fmin(double x, double y)
{
    if (isnan(x)) {
        if (__issignaling(x) != 0 || __issignaling(y) != 0) {
            return x * y;
        }
#ifdef __LIBMCS_FPU_DAZ
        y *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */
        return y;
    }

    if (isnan(y)) {
        if (__issignaling(y) != 0) {
            return x * y;
        }
#ifdef __LIBMCS_FPU_DAZ
        x *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */
        return x;
    }
    
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_one;
    y *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    return x < y ? x : y;
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double fminl(long double x, long double y)
{
    return (long double) fmin((double) x, (double) y);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
