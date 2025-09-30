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
 *     float roundf(float x);
 *     double round(double x);
 *     long double roundl(long double x);
 *
 * Description
 * ===========
 *
 * ``round`` computes the nearest integer value to :math:`x`. Half-way cases
 * are rounded away from zero (which is the only difference to
 * :ref:`nearbyint`, :ref:`rint`).
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    round(x) = \lfloor x \rceil
 *
 * Returns
 * =======
 *
 * ``round`` returns the nearest integer value to :math:`x`.
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
 * | **round(x)**        | :math:`-Inf` | :math:`\lfloor x \rceil` | :math:`x`                   | :math:`\lfloor x \rceil` | :math:`+Inf` | :math:`qNaN` |
 * +---------------------+--------------+--------------------------+--------------+--------------+--------------------------+--------------+--------------+
 *
 */

#include <math.h>
#include "../common/tools.h"

#ifndef __LIBMCS_DOUBLE_IS_32BITS

double round(double x)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    /* Most significant word, least significant word. */
    int32_t msw, exponent_less_1023;
    uint32_t lsw;

    EXTRACT_WORDS(msw, lsw, x);

    /* Extract exponent field. */
    exponent_less_1023 = ((msw & 0x7ff00000) >> 20) - 1023;

    if (exponent_less_1023 < 20) {
        if (exponent_less_1023 < 0) {
            msw &= 0x80000000U;

            if (exponent_less_1023 == -1) {                 /* Result is +1.0 or -1.0. */
                msw |= ((int32_t)1023 << 20);
            }

            lsw = 0;
        } else {
            uint32_t exponent_mask = 0x000fffff >> exponent_less_1023;

            if ((msw & exponent_mask) == 0 && lsw == 0) {   /* x in an integral value. */
                return x;
            }

            msw += 0x00080000 >> exponent_less_1023;
            msw &= ~exponent_mask;
            lsw = 0;
        }
    } else if (exponent_less_1023 > 51) {
        if (exponent_less_1023 == 1024) {                   /* x is NaN or infinite. */
            return x + x;
        } else {
            return x;
        }
    } else {
        uint32_t exponent_mask = 0xffffffffU >> (exponent_less_1023 - 20);
        uint32_t tmp;

        if ((lsw & exponent_mask) == 0) {                   /* x is an integral value. */
            return x;
        }

        tmp = lsw + (1 << (51 - exponent_less_1023));

        if (tmp < lsw) {
            msw += 1;
        }

        lsw = tmp;

        lsw &= ~exponent_mask;
    }

    INSERT_WORDS(x, msw, lsw);

    return x;
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double roundl(long double x)
{
    return (long double) round((double) x);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
