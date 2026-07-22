/* SPDX-License-Identifier: GTDGmbH */
/* Copyright 2020-2025 by GTD GmbH. */

/**
 *
 * This macro is used to test if :math:`x` is a finite floating-point value. It
 * can be called with ``float``, ``double`` or ``long double`` input. This
 * macro is implemented in ``math.h``.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     int isfinite(x);
 *
 * Description
 * ===========
 *
 * ``isfinite`` tests whether the input value is finite (neither infinite nor
 * :math:`NaN`) or not.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    isfinite(x) = \left\{\begin{array}{ll} 1, & x \in \mathbb{F} \setminus \left \{ \pm Inf, NaN \right \} \\
 *                                           0, & otherwise \end{array}\right.
 *
 * Returns
 * =======
 *
 * ``isfinite`` returns :math:`1` if the input is finite, else :math:`0`.
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
 * | **isfinite(x)**     | :math:`0`    | :math:`1`                                                        | :math:`0`    | :math:`0`   |
 * +---------------------+--------------+------------------------------------------------------------------+--------------+-------------+
 *
 */

