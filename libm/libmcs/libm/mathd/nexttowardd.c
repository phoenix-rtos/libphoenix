/* SPDX-License-Identifier: GTDGmbH */
/* Copyright 2020-2025 by GTD GmbH. */

/**
 *
 * This family of functions computes the next floating-point value after
 * :math:`x` in direction of :math:`y`.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     float nexttowardf(float x, long double y);
 *     double nexttoward(double x, long double y);
 *     long double nexttowardl(long double x, long double y);
 *
 * Description
 * ===========
 *
 * ``nexttoward`` computes the next floating-point value after :math:`x` in
 * direction of :math:`y`.
 *
 * ``nexttoward`` is functionally equivalent to :ref:`nextafter` but uses
 * ``long double`` for the :math:`y` input. As the library only supports 64bit
 * ``long double`` the procedure ``nexttoward`` is just calling
 * :ref:`nextafter` as the types are the same, but for ``nexttowardf`` there is
 * an actual difference as :math:`y` can be chosen to be so close to :math:`x`
 * that it would be the same value in ``float`` but isn't in (effectively)
 * ``double``. This results in ``nexttowardf`` having it's own implementation
 * while ``nexttoward`` does not.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    nexttoward(x, y) = \left\{\begin{array}{ll} y, & x = y \\ \text{min} \{z | z \in \mathbb{F} \wedge z > x\}, & x < y \\ \text{max} \{z | z \in \mathbb{F} \wedge z < x\}, & otherwise \end{array}\right.
 *
 * Returns
 * =======
 *
 * ``nexttoward`` returns the the next floating-point value after :math:`x` in
 * direction of :math:`y`.
 *
 * Exceptions
 * ==========
 *
 * Raise ``overflow`` exception when the magnitude of :math:`x` is the largest
 * finite value representable in the type and :math:`y` is infinite with the
 * same sign as :math:`x`.
 *
 * .. May raise ``underflow`` exception.
 *
 * Output map
 * ==========
 *
 * +--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+
 * | nexttoward(x,y)          | x                                                                                                                                                                                          |
 * +--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+
 * | y                        | :math:`-Inf`             | :math:`<0`               | :math:`-0`               | :math:`+0`               | :math:`>0`               | :math:`+Inf`             | :math:`NaN`              |
 * +==========================+==========================+==========================+==========================+==========================+==========================+==========================+==========================+
 * | :math:`-Inf`             | :math:`-Inf`             | :math:`nexttoward(x, y)` | max :math:`\mathbb{S}^{-}`                          | :math:`nexttoward(x, y)` | :math:`nexttoward(x, y)` | :math:`qNaN`             |
 * +--------------------------+--------------------------+                          +                                                     +                          +                          +                          +
 * | :math:`<0`               | :math:`nexttoward(x, y)` |                          |                                                     |                          |                          |                          |
 * +--------------------------+                          +                          +--------------------------+--------------------------+                          +                          +                          +
 * | :math:`-0`               |                          |                          | :math:`y`                                           |                          |                          |                          |
 * +--------------------------+                          +                          +                                                     +                          +                          +                          +
 * | :math:`+0`               |                          |                          |                                                     |                          |                          |                          |
 * +--------------------------+                          +                          +--------------------------+--------------------------+                          +                          +                          +
 * | :math:`>0`               |                          |                          | min :math:`\mathbb{S}^{+}`                          |                          |                          |                          |
 * +--------------------------+                          +                          +                                                     +                          +--------------------------+                          +
 * | :math:`+Inf`             |                          |                          |                                                     |                          | :math:`+Inf`             |                          |
 * +--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+                          +
 * | :math:`NaN`              | :math:`qNaN`                                                                                                                                                    |                          |
 * +--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+
 *
 */

#ifndef __LIBMCS_DOUBLE_IS_32BITS

#include <math.h>

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

double nexttoward(double x, long double y)
{
    return nextafter(x, (double) y);
}

long double nexttowardl(long double x, long double y)
{
    return (long double) nexttoward((double) x, y);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
