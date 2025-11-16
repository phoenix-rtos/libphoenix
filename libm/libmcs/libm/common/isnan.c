/* SPDX-License-Identifier: GTDGmbH */
/* Copyright 2020-2025 by GTD GmbH. */

/**
 *
 * This macro is used to test if :math:`x` is a :math:`NaN`. It can be called
 * with ``float``, ``double`` or ``long double`` input. This macro is
 * implemented in ``math.h``.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     int isnan(x);
 *
 * Description
 * ===========
 *
 * ``isnan`` tests whether the input value is a :math:`NaN` or not.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    isnan(x) = \left\{\begin{array}{ll} 1, & x = NaN \\
 *                                        0, & otherwise \end{array}\right.
 *
 * Returns
 * =======
 *
 * ``isnan`` returns :math:`1` if the input is a :math:`NaN`, else :math:`0`.
 *
 * Exceptions
 * ==========
 *
 * Does not raise exceptions.
 *
 * Output map
 * ==========
 *
 * +---------------------+--------------+------------------------------------------------------------------+--------------+-------------+
 * | **x**               | :math:`-Inf` | :math:`\in \mathbb{F} \setminus \left \{ \pm Inf, NaN \right \}` | :math:`+Inf` | :math:`NaN` |
 * +=====================+==============+==================================================================+==============+=============+
 * | **isnan(x)**        | :math:`0`    | :math:`0`                                                        | :math:`0`    | :math:`1`   |
 * +---------------------+--------------+------------------------------------------------------------------+--------------+-------------+
 *
 */

