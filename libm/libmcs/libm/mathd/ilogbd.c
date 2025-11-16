/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */

/**
 *
 * This family of functions computes the binary exponent of the input value as
 * a signed integer.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     int ilogbf(float x);
 *     int ilogb(double x);
 *     int ilogbl(long double x);
 *
 * Description
 * ===========
 *
 * ``ilogb`` computes the binary exponent of the input value as a signed integer.
 *
 * ``ilogb`` and :ref:`logb` have the same functionality, but ``ilogb`` returns
 * the binary exponent as a signed integer while :ref:`logb` returns a
 * floating-point value.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    ilogb(x) \approx \lfloor {\log_2 |x|} \rfloor
 *
 * Returns
 * =======
 *
 * ``ilogb`` returns the binary exponent of the input value, in the range
 * :math:`\mathbb{I}`.
 *
 * Exceptions
 * ==========
 *
 * Raise ``invalid operation`` exception when the correct result is not
 * representable as an integer. This is the case when the input value is zero,
 * infinite or :math:`NaN`, or the magnitude of the result is too large to be
 * represented as an integer.
 *
 * Output map
 * ==========
 *
 * +---------------------+------------------------+----------------------------------------------------------------------------------------------------------+------------------------------------------------------------------+----------------------------------------------------------------------------------------------------------+--------------+--------------+
 * | **x**               | :math:`-Inf`           | :math:`<0 \wedge \lfloor {\log_2 |x|} \rfloor \notin \mathbb{I} \wedge \lfloor {\log_2 |x|} \rfloor > 0` | :math:`<0 \wedge \lfloor {\log_2 |x|} \rfloor \in \mathbb{I}`    | :math:`<0 \wedge \lfloor {\log_2 |x|} \rfloor \notin \mathbb{I} \wedge \lfloor {\log_2 |x|} \rfloor < 0` | :math:`-0`   | :math:`+0`   |
 * +=====================+========================+==========================================================================================================+==================================================================+==========================================================================================================+==============+==============+
 * | **ilogb(x)**        | max :math:`\mathbb{I}` | max :math:`\mathbb{I}`                                                                                   | :math:`\lfloor {\log_2 |x|} \rfloor`                             | min :math:`\mathbb{I}`                                                                                   | min :math:`\mathbb{I}`      |
 * +---------------------+------------------------+----------------------------------------------------------------------------------------------------------+------------------------------------------------------------------+----------------------------------------------------------------------------------------------------------+--------------+--------------+
 *
 * +---------------------+----------------------------------------------------------------------------------------------------------+------------------------------------------------------------------+----------------------------------------------------------------------------------------------------------+--------------+--------------+
 * | **x**               | :math:`>0 \wedge \lfloor {\log_2 |x|} \rfloor \notin \mathbb{I} \wedge \lfloor {\log_2 |x|} \rfloor < 0` | :math:`<0 \wedge \lfloor {\log_2 |x|} \rfloor \in \mathbb{I}`    | :math:`>0 \wedge \lfloor {\log_2 |x|} \rfloor \notin \mathbb{I} \wedge \lfloor {\log_2 |x|} \rfloor > 0` | :math:`+Inf` | :math:`NaN`  |
 * +=====================+==========================================================================================================+==================================================================+==========================================================================================================+==============+==============+
 * | **ilogb(x)**        | min :math:`\mathbb{I}`                                                                                   | :math:`\lfloor {\log_2 |x|} \rfloor`                             | max :math:`\mathbb{I}`                                                                                   | max :math:`\mathbb{I}`      |
 * +---------------------+----------------------------------------------------------------------------------------------------------+------------------------------------------------------------------+----------------------------------------------------------------------------------------------------------+--------------+--------------+
 *
 * .. raw:: html
 *
 *    <!--
 *    TODO: Check usefulness of this output map as well as the exception, as I
 *    think the magnitude of the result can only exceed the integer range if
 *    the size of ``int`` is smaller than [-1074,1023]. So the POSIX/C/etc.
 *    issues may not apply to our use-cases.
 *    -->
 *
 */

#include <math.h>
#include "../common/tools.h"

#ifndef __LIBMCS_DOUBLE_IS_32BITS

int ilogb(double x)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    int32_t hx, lx, ix;

    EXTRACT_WORDS(hx, lx, x);
    hx &= 0x7fffffff;

    if (hx < 0x00100000) {
        if ((hx | lx) == 0) {
            (void) __raise_invalid();
            return FP_ILOGB0;    /* ilogb(0) = special case error */
        } else {         /* subnormal x */
            if (hx == 0) {
                for (ix = -1043; lx > 0; lx <<= 1) {
                    ix -= 1;
                }
            } else {
                for (ix = -1022, hx <<= 11; hx > 0; hx <<= 1) {
                    ix -= 1;
                }
            }
        }

        return ix;
    } else if (hx < 0x7ff00000) {
        return (hx >> 20) - 1023;
    } else if (hx > 0x7ff00000) {
        (void) __raise_invalid();
        return FP_ILOGBNAN;     /* NAN */
    } else {
        (void) __raise_invalid();
        return INT_MAX;         /* infinite */
    }
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

int ilogbl(long double x)
{
    return ilogb((double) x);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
