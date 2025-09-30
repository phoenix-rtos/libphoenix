/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */
/* Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com. */

#include <math.h>
#include "../common/tools.h"

static const float one = 1.0f;

float atanhf(float x)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_onef;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    float t;
    int32_t hx, ix;
    GET_FLOAT_WORD(hx, x);
    ix = hx & 0x7fffffff;

    if (ix > 0x3f800000) {     /* |x|>1 */
        if (isnan(x)) {
            return x + x;
        } else {
            return __raise_invalidf();
        }
    }

    if (ix == 0x3f800000) {
        return __raise_div_by_zerof(x);
    }

    if (ix < 0x31800000) {    /* x<2**-28 */
        if (FLT_UWORD_IS_ZERO(ix)) {    /* return x inexact except 0 */
            return x;
        } else {
            return __raise_inexactf(x);
        }
    }

    SET_FLOAT_WORD(x, ix);

    if (ix < 0x3f000000) {     /* x < 0.5 */
        t = x + x;
        t = 0.5f * log1pf(t + t * x / (one - x));
    } else {
        t = 0.5f * log1pf((x + x) / (one - x));
    }

    if (hx >= 0) {
        return t;
    } else {
        return -t;
    }
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double atanh(double x)
{
    return (double) atanhf((float) x);
}

#endif /* #ifdef __LIBMCS_DOUBLE_IS_32BITS */
