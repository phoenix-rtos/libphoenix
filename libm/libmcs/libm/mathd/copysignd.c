/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */

/**
 *
 * This family of functions copies the sign of :math:`y` onto :math:`x`.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     float copysignf(float x, float y);
 *     double copysign(double x, double y);
 *     long double copysignl(long double x, long double y);
 *
 * Description
 * ===========
 *
 * ``copysign`` computes the value with the magnitude of :math:`x` and sign of
 * :math:`y`.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    copysign(x, y) = |x| \cdot sgn\ y
 *
 * Returns
 * =======
 *
 * ``copysign`` returns the value with the magnitude of :math:`x` and sign of
 * :math:`y`.
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
 * +--------------------------+--------------------------+--------------------------+
 * | copysign(x,y)            | x                                                   |
 * +--------------------------+--------------------------+--------------------------+
 * | y                        | :math:`\neq NaN`         | :math:`NaN`              |
 * +==========================+==========================+==========================+
 * | :math:`-NaN`             | :math:`-|x|`             | :math:`qNaN`             |
 * +--------------------------+                          +                          +
 * | :math:`-Inf`             |                          |                          |
 * +--------------------------+                          +                          +
 * | :math:`<0`               |                          |                          |
 * +--------------------------+                          +                          +
 * | :math:`-0`               |                          |                          |
 * +--------------------------+--------------------------+                          +
 * | :math:`+0`               | :math:`|x|`              |                          |
 * +--------------------------+                          +                          +
 * | :math:`>0`               |                          |                          |
 * +--------------------------+                          +                          +
 * | :math:`+Inf`             |                          |                          |
 * +--------------------------+                          +                          +
 * | :math:`+NaN`             |                          |                          |
 * +--------------------------+--------------------------+--------------------------+
 *
 */

#include <math.h>
#include "../common/tools.h"

#ifndef __LIBMCS_DOUBLE_IS_32BITS

double copysign(double x, double y)
{
    uint32_t hx, hy;
    GET_HIGH_WORD(hx, x);
    GET_HIGH_WORD(hy, y);
    SET_HIGH_WORD(x, (hx & 0x7fffffffU) | (hy & 0x80000000U));
    return x;
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double copysignl(long double x, long double y)
{
    return (long double) copysign((double) x, (double) y);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
