/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */
/* Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com. */

#include <math.h>

float ldexpf(float x, int exp)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_onef;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    if ((isfinite(x) == 0) || x == 0.0f) {
        return x + x;
    }

    return scalbnf(x, exp);
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double ldexp(double x, int exp)
{
    return (double) ldexpf((float) x, exp);
}

#endif /* #ifdef __LIBMCS_DOUBLE_IS_32BITS */
