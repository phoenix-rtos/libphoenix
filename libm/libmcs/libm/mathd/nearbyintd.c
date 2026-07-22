/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */

/**
 *
 * This family of functions implements the nearest integer value to :math:`x`.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     float nearbyintf(float x);
 *     double nearbyint(double x);
 *     long double nearbyintl(long double x);
 *
 * Description
 * ===========
 *
 * ``nearbyint`` computes the nearest integer value to :math:`x`.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    nearbyint(x) = \lfloor x \rceil
 *
 * Returns
 * =======
 *
 * ``nearbyint`` returns the nearest integer value to :math:`x`.
 *
 * Exceptions
 * ==========
 *
 * Does not raise exceptions.
 *
 * Output map
 * ==========
 *
 * +---------------------+--------------+--------------------------+--------------+--------------+--------------------------+--------------+--------------+
 * | **x**               | :math:`-Inf` | :math:`<0`               | :math:`-0`   | :math:`+0`   | :math:`>0`               | :math:`+Inf` | :math:`NaN`  |
 * +=====================+==============+==========================+==============+==============+==========================+==============+==============+
 * | **nearbyint(x)**    | :math:`-Inf` | :math:`\lfloor x \rceil` | :math:`x`                   | :math:`\lfloor x \rceil` | :math:`+Inf` | :math:`qNaN` |
 * +---------------------+--------------+--------------------------+--------------+--------------+--------------------------+--------------+--------------+
 *
 */

#include <math.h>

#ifndef __LIBMCS_DOUBLE_IS_32BITS

double nearbyint(double x)
{
    return rint(x);
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double nearbyintl(long double x)
{
    return (long double) nearbyint((double) x);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
