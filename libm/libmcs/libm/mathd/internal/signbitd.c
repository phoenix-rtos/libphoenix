/* SPDX-License-Identifier: RedHat */
/* Copyright (C) 2002 by  Red Hat, Incorporated. All rights reserved. */

/**
 *
 * This macro returns a non-zero value if the sign bit of :math:`x` is set. It
 * can be called with ``float``, ``double`` or ``long double`` input. This
 * macro is implemented in ``math.h``.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     int signbit(x);
 *
 * Description
 * ===========
 *
 * ``signbit`` returns a non-zero value if the sign bit of :math:`x` is set.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    signbit(x) = \left\{\begin{array}{ll} 1, & x \in \mathbb{F}^{-} \\
 *                                          0, & otherwise \end{array}\right.
 *
 * Returns
 * =======
 *
 * ``signbit`` returns :math:`1` if the sign bit of :math:`x` is set, otherwise
 * :math:`0`.
 *
 * Exceptions
 * ==========
 *
 * Does not raise exceptions.
 *
 * Output map
 * ==========
 *
 * +---------------------+--------------+--------------+--------------+--------------+--------------+--------------+--------------+--------------+
 * | **x**               | :math:`-NaN` | :math:`-Inf` | :math:`<0`   | :math:`-0`   | :math:`+0`   | :math:`>0`   | :math:`+Inf` | :math:`+NaN` |
 * +=====================+==============+==============+==============+==============+==============+==============+==============+==============+
 * | **signbit(x)**      | :math:`1`                                                 | :math:`0`                                                 |
 * +---------------------+--------------+--------------+--------------+--------------+--------------+--------------+--------------+--------------+
 *
 */

#include <math.h>
#include "../../common/tools.h"

int __signbitd(double x)
{
    uint32_t msw;

    GET_HIGH_WORD(msw, x);

    return (msw & 0x80000000U) != 0;
}
