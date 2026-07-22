/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */

/**
 *
 * This family of functions implements the floating-point remainder :math:`x\
 * REM\ y` and puts the quotient (or rather its sign and 3 least significant
 * bits) into the out pointer.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     float remquof(float x, float y, int *quo);
 *     double remquo(double x, double y, int *quo);
 *     long double remquol(long double x, long double y, int *quo);
 *
 * Description
 * ===========
 *
 * ``remquo`` computes the floating-point remainder :math:`r = x\ REM\ y = x -
 * n \cdot y` of their arguments :math:`x` and :math:`y`, where :math:`n` is
 * the integral value nearest to :math:`\frac{x}{y}`.
 *
 * The 3 least significant bits of :math:`n` and its sign are then put into the
 * output pointer :math:`*quo`.
 *
 * ``remquo`` and ``remainder`` are functionally equivalent concerning the
 * return value, ``remquo`` only adds an additional output.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    remquo(x, y) = x - n \cdot y \wedge n \in \mathbb{Z} \wedge remquo(x, y) \in \left [-\left | \frac{y}{2} \right |,\left | \frac{y}{2} \right | \right ] \wedge quo = n
 *
 * Returns
 * =======
 *
 * ``remquo`` returns the floating-point remainder :math:`x\ REM\ y`. The sign
 * and 3 least significant bits of quotient :math:`n` is put into :math:`*quo`.
 *
 * Exceptions
 * ==========
 *
 * Raise ``invalid operation`` exception when :math:`x` is infinite or
 * :math:`y` is zero.
 *
 * .. May raise ``underflow`` exception.
 *
 * Output map
 * ==========
 *
 * +--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+
 * | remquo(x,y)              | x                                                                                                                                                                                          |
 * +--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+
 * | y                        | :math:`-Inf`             | :math:`<0`               | :math:`-0`               | :math:`+0`               | :math:`>0`               | :math:`+Inf`             | :math:`NaN`              |
 * +==========================+==========================+==========================+==========================+==========================+==========================+==========================+==========================+
 * | :math:`-Inf`             | :math:`qNaN`             | :math:`x`                                                                                                 | :math:`qNaN`             | :math:`qNaN`             |
 * +--------------------------+                          +--------------------------+--------------------------+--------------------------+--------------------------+                          +                          +
 * | :math:`<0`               |                          | :math:`x\ REM\ y`        | :math:`x`                                           | :math:`x\ REM\ y`        |                          |                          |
 * +--------------------------+                          +--------------------------+--------------------------+--------------------------+--------------------------+                          +                          +
 * | :math:`-0`               |                          | :math:`qNaN`                                                                                              |                          |                          |
 * +--------------------------+                          +                                                                                                           +                          +                          +
 * | :math:`+0`               |                          |                                                                                                           |                          |                          |
 * +--------------------------+                          +--------------------------+--------------------------+--------------------------+--------------------------+                          +                          +
 * | :math:`>0`               |                          | :math:`x\ REM\ y`        | :math:`x`                                           | :math:`x\ REM\ y`        |                          |                          |
 * +--------------------------+                          +--------------------------+--------------------------+--------------------------+--------------------------+                          +                          +
 * | :math:`+Inf`             |                          | :math:`x`                                                                                                 |                          |                          |
 * +--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+                          +
 * | :math:`NaN`              | :math:`qNaN`                                                                                                                                                    |                          |
 * +--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+
 *
 * +--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+
 * | :math:`*quo`             | x                                                                                                                                                                                          |
 * +--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+
 * | y                        | :math:`-Inf`             | :math:`<0`               | :math:`-0`               | :math:`+0`               | :math:`>0`               | :math:`+Inf`             | :math:`NaN`              |
 * +==========================+==========================+==========================+==========================+==========================+==========================+==========================+==========================+
 * | :math:`-Inf`             | :math:`0`                | :math:`0`                                                                                                 | :math:`0`                | :math:`0`                |
 * +--------------------------+                          +--------------------------+--------------------------+--------------------------+--------------------------+                          +                          +
 * | :math:`<0`               |                          | :math:`n`                | :math:`0`                                           | :math:`n`                |                          |                          |
 * +--------------------------+                          +--------------------------+--------------------------+--------------------------+--------------------------+                          +                          +
 * | :math:`-0`               |                          | :math:`0`                                                                                                 |                          |                          |
 * +--------------------------+                          +                                                                                                           +                          +                          +
 * | :math:`+0`               |                          |                                                                                                           |                          |                          |
 * +--------------------------+                          +--------------------------+--------------------------+--------------------------+--------------------------+                          +                          +
 * | :math:`>0`               |                          | :math:`n`                | :math:`0`                                           | :math:`n`                |                          |                          |
 * +--------------------------+                          +--------------------------+--------------------------+--------------------------+--------------------------+                          +                          +
 * | :math:`+Inf`             |                          | :math:`0`                                                                                                 |                          |                          |
 * +--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+                          +
 * | :math:`NaN`              | :math:`0`                                                                                                                                                       |                          |
 * +--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+
 *
 */

#include <assert.h>
#include <math.h>
#include "../common/tools.h"

#ifndef __LIBMCS_DOUBLE_IS_32BITS

static const double zero = 0.0;

double remquo(double x, double y, int *quo)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_one;
    y *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    int _quo = 0;
    int32_t hx, hy;
    uint32_t sx, sq, lx, ly;
    double y_half;

    assert(quo != (void*)0);
    if(quo == (void*)0) {
        quo = &_quo;
    }
    *quo = 0;

    EXTRACT_WORDS(hx, lx, x);
    EXTRACT_WORDS(hy, ly, y);
    sx = hx & 0x80000000U;
    sq = sx ^ (hy & 0x80000000U);
    hy &= 0x7fffffff;
    hx &= 0x7fffffff;

    /* purge off exception values */
    if ((hx >= 0x7ff00000) || (hy >= 0x7ff00000)) { /* x or y not finite */
        if (isnan(x) || isnan(y)) {                 /* x or y is NaN */
            return x + y;
        } else if (hx == 0x7ff00000) {              /* x is infinite */
            return __raise_invalid();
        } else {
            /* No action required */
        }
    } else if ((hy | ly) == 0) {                    /* y = 0 */
        return __raise_invalid();
    } else {
        /* No action required */
    }

    if (hy <= 0x7fbfffff) {
        x = fmod(x, 8 * y);                         /* now x < 8y */
    }

    if (((hx - hy) | (lx - ly)) == 0) {             /* x equals y */
        *quo = sq ? -1 : 1;
        return zero * x;
    }

    x  = fabs(x);
    y  = fabs(y);
    _quo = 0;

    if (x >= 4 * y) {
        x -= 4 * y;
        _quo += 4;
    }
    if (x >= 2 * y) {
        x -= 2 * y;
        _quo += 2;
    }

    if (hy < 0x00200000) {
        if (x + x > y) {
            x -= y;
            _quo++;

            if (x + x >= y) {
                x -= y;
                _quo++;
            }
        }
    } else {
        y_half = 0.5 * y;

        if (x > y_half) {
            x -= y;
            _quo++;

            if (x >= y_half) {
                x -= y;
                _quo++;
            }
        }
    }

    _quo &= 0x7;

    *quo = sq ? -_quo : _quo;

    GET_HIGH_WORD(hx, x);
    SET_HIGH_WORD(x, hx ^ sx);
    return x;
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double remquol(long double x, long double y, int *quo)
{
    return (long double) remquo((double) x, (double) y, quo);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
