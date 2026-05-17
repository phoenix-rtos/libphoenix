/* SPDX-License-Identifier: GTDGmbH */
/* Copyright 2020-2025 by GTD GmbH. */

/**
 *
 * This macro is used to test if :math:`x` is a normal floating-point value. It
 * can be called with ``float``, ``double`` or ``long double`` input. This
 * macro is implemented in ``math.h``.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     int isnormal(x);
 *
 * Description
 * ===========
 *
 * ``isnormal`` tests whether the input value is a normal value or not.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    isnormal(x) = \left\{\begin{array}{ll} 1, & x \in \mathbb{F} \setminus (\left \{ \pm 0.0, \pm Inf, NaN \right \} \cup \mathbb{S}) \\
 *                                           0, & otherwise \end{array}\right.
 *
 * Returns
 * =======
 *
 * ``isnormal`` returns :math:`1` if the input is a normal value, else :math:`0`.
 *
 * Exceptions
 * ==========
 *
 * Does not raise exceptions.
 *
 * Output map
 * ==========
 *
 * +---------------------+--------------+---------------------------------------------------------------------------------------------+-----------------+------------------------+--------------+-------------+
 * | **x**               | :math:`-Inf` | :math:`\in \mathbb{F} \setminus (\left \{ \pm 0.0, \pm Inf, NaN \right \} \cup \mathbb{S})` | :math:`\pm 0.0` | :math:`\in \mathbb{S}` | :math:`+Inf` | :math:`NaN` |
 * +=====================+==============+=============================================================================================+=================+========================+==============+=============+
 * | **isnormal(x)**     | :math:`0`    | :math:`1`                                                                                   | :math:`0`       | :math:`0`              | :math:`0`    | :math:`0`   |
 * +---------------------+--------------+---------------------------------------------------------------------------------------------+-----------------+------------------------+--------------+-------------+
 *
 */

