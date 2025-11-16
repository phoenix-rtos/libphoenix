/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */

/**
 *
 * This family of functions implements the absolute value of :math:`x`.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     float fabsf(float x);
 *     double fabs(double x);
 *     long double fabsl(long double x);
 *
 * Description
 * ===========
 *
 * ``fabs`` computes the absolute value of the input value.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    fabs(x) = |x|
 *
 * Returns
 * =======
 *
 * ``fabs`` returns the absolute value of the input value.
 *
 * ``fabs`` does not differentiate between different types of ``NaN``, as such
 * if the input is ``sNaN`` so will the output, only the sign bit may change.
 *
 * Exceptions
 * ==========
 *
 * Does not raise exceptions.
 *
 * Does not raise ``invalid operation`` exception on ``sNaN`` input.
 *
 * Output map
 * ==========
 *
 * +---------------------+--------------+---------------------+--------------+--------------+---------------------+--------------+--------------+--------------+
 * | **x**               | :math:`-Inf` | :math:`<0`          | :math:`-0`   | :math:`+0`   | :math:`>0`          | :math:`+Inf` | :math:`qNaN` | :math:`sNaN` |
 * +=====================+==============+=====================+==============+==============+=====================+==============+==============+==============+
 * | **fabs(x)**         | :math:`+Inf` | :math:`-x`          | :math:`+0`                  | :math:`x`           | :math:`+Inf` | :math:`qNaN` | :math:`sNaN` |
 * +---------------------+--------------+---------------------+--------------+--------------+---------------------+--------------+--------------+--------------+
 *
 */

#include <math.h>
#include "../common/tools.h"

#ifndef __LIBMCS_DOUBLE_IS_32BITS

double fabs(double x)
{
    uint32_t high;
    GET_HIGH_WORD(high, x);
    SET_HIGH_WORD(x, high & 0x7fffffff);
    return x;
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double fabsl(long double x)
{
    return (long double) fabs((double) x);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
