/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */
/* Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com. */

/*
 * floorf(x)
 * Return x rounded toward -inf to integral value
 * Method:
 *    Bit twiddling.
 * Exception:
 *    Inexact flag raised if x not equal to floorf(x).
 */

#include <math.h>
#include "../common/tools.h"

float floorf(float x)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_onef;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    int32_t _i0, _j0;
    uint32_t i, ix;
    GET_FLOAT_WORD(_i0, x);
    ix = (_i0 & 0x7fffffff);
    _j0 = (ix >> 23) - 0x7f;

    if (_j0 < 23) {
        if (_j0 < 0) {  /* raise inexact if x != 0 */
            if (FLT_UWORD_IS_ZERO(ix)) {
                return x;
            }

            (void) __raise_inexactf(x);

            if (_i0 >= 0) {
                _i0 = 0;
            } else {
                _i0 = (int32_t)0xbf800000U;
            }
        } else {
            i = (0x007fffff) >> _j0;

            if ((_i0 & i) == 0) {    /* x is integral */
                return x;
            }

            (void) __raise_inexactf(x);

            if (_i0 < 0) {
                _i0 += (0x00800000) >> _j0;
            }

            _i0 &= (~i);
        }
    } else {
        if (!FLT_UWORD_IS_FINITE(ix)) {
            return x + x;    /* inf or NaN */
        } else {
            return x;    /* x is integral */
        }
    }

    SET_FLOAT_WORD(x, _i0);
    return x;
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double floor(double x)
{
    return (double) floorf((float) x);
}

#endif /* #ifdef __LIBMCS_DOUBLE_IS_32BITS */
