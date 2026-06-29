/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */
/* Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com. */

#include <math.h>
#include "../common/tools.h"

int ilogbf(float x)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_onef;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    int32_t hx, ix;

    GET_FLOAT_WORD(hx, x);
    hx &= 0x7fffffff;

    if (FLT_UWORD_IS_ZERO(hx)) {
        (void) __raise_invalidf();
        return FP_ILOGB0;    /* ilogb(0) = special case error */
    } else if (FLT_UWORD_IS_SUBNORMAL(hx)) {
        for (ix = -126, hx <<= 8; hx > 0; hx <<= 1) {
            ix -= 1;
        }

        return ix;
    } else if (FLT_UWORD_IS_FINITE(hx)) {
        return (hx >> 23) - 127;
    } else if (FLT_UWORD_IS_NAN(hx)) {
        (void) __raise_invalidf();
        return FP_ILOGBNAN;     /* NAN */
    } else {
        (void) __raise_invalidf();
        return INT_MAX;         /* infinite */
    }
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

int ilogb(double x)
{
    return ilogbf((float) x);
}

#endif /* #ifdef __LIBMCS_DOUBLE_IS_32BITS */
