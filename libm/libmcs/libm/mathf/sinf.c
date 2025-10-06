/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */
/* Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com. */

#include <math.h>
#include "../common/tools.h"
#include "internal/trigf.h"

float sinf(float x)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_onef;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    float y[2], z = 0.0f;
    int32_t n, ix;

    GET_FLOAT_WORD(ix, x);

    /* |x| ~< pi/4 */
    ix &= 0x7fffffff;

    if (ix <= 0x3f490fd8) {
        if(ix < 0x39800000) {        /* if x < 2**-12 */
            if (FLT_UWORD_IS_ZERO(ix)) {    /* return x inexact except 0 */
                return x;
            } else {
                return __raise_inexactf(x);
            }
        }
        return __sinf(x, z, 0);
    }

    /* sin(Inf or NaN) is NaN */
    else if (!FLT_UWORD_IS_FINITE(ix)) {
        if (isnan(x)) {
            return x + x;
        } else {
            return __raise_invalidf();
        }
    }

    /* argument reduction needed */
    else {
        n = __rem_pio2f(x, y);

        switch (n & 3) {
        case 0:
            return  __sinf(y[0], y[1], 1);

        case 1:
            return  __cosf(y[0], y[1]);

        case 2:
            return -__sinf(y[0], y[1], 1);

        default:
            return -__cosf(y[0], y[1]);
        }
    }
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double sin(double x)
{
    return (double) sinf((float) x);
}

#endif /* #ifdef __LIBMCS_DOUBLE_IS_32BITS */
