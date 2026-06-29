/* SPDX-License-Identifier: GTDGmbH */
/* Copyright 2020-2025 by GTD GmbH. */

/**
 *
 * This macro is used to test if :math:`x` or :math:`y` are ``NaN``.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     int isunordered(x, y);
 *
 * Description
 * ===========
 *
 * ``isunordered`` tests whether :math:`x` or :math:`y` are ``NaN`` as ``NaN``
 * values are neither less than, greater than, nor equal to another value, and
 * as such cannot be ordered.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    isunordered(x, y) = \left\{\begin{array}{ll} 1, & x = \text{NaN} \vee y = \text{NaN} \\
 *                                                 0, & otherwise \end{array}\right.
 *
 * Returns
 * =======
 *
 * ``isunordered`` returns :math:`1` if either input is ``NaN``, else :math:`0`.
 *
 * Exceptions
 * ==========
 *
 * Does not raise exceptions. This includes ``sNaN`` inputs.
 *
 * Output map
 * ==========
 *
 * +--------------------------+--------------------------+--------------------------+
 * | isunordered(x,y)         | x                                                   |
 * +--------------------------+--------------------------+--------------------------+
 * | y                        | :math:`\neq NaN`         | :math:`NaN`              |
 * +==========================+==========================+==========================+
 * | :math:`\neq NaN`         | :math:`0`                | :math:`1`                |
 * +--------------------------+--------------------------+                          +
 * | :math:`NaN`              | :math:`1`                |                          |
 * +--------------------------+--------------------------+--------------------------+
 *
 */
