/* SPDX-License-Identifier: GTDGmbH */
/* Copyright 2020-2025 by GTD GmbH. */

/**
 *
 * This macro is used to test if :math:`x` is an infinite floating-point value.
 * It can be called with ``float``, ``double`` or ``long double`` input. This
 * macro is implemented in ``math.h``.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     int isinf(x);
 *
 * Description
 * ===========
 *
 * ``isinf`` tests whether the input value is infinite or not.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    isinf(x) = \left\{\begin{array}{ll} 1, & x = \pm Inf  \\
 *                                        0, & otherwise \end{array}\right.
 *
 * Returns
 * =======
 *
 * ``isinf`` returns :math:`1` if the input is infinite, else :math:`0`.
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
 * | **isinf(x)**        | :math:`1`    | :math:`0`                                                        | :math:`1`    | :math:`0`   |
 * +---------------------+--------------+------------------------------------------------------------------+--------------+-------------+
 *
 */

