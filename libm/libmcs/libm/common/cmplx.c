/* SPDX-License-Identifier: GTDGmbH */
/* Copyright 2020-2025 by GTD GmbH. */

/**
 *
 * This family of macros is used to create a complex value out of the two
 * inputs where :math:`x` is real and :math:`y` is the imaginary part.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     float complex CMPLXF(float x, float y);
 *     double complex CMPLX(double x, double y);
 *     long double complex CMPLXL(long double x, long double y);
 *
 * Description
 * ===========
 *
 * ``CMPLX`` creates a complex value out of the two inputs where :math:`x` is
 * real and :math:`y` is the imaginary part.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    CMPLX(x, y) = x + iy
 *
 * Returns
 * =======
 *
 * ``CMPLX`` returns a complex value where :math:`x` is real and :math:`y` is
 * the imaginary part.
 *
 */
