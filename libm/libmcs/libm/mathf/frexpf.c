/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */
/* Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com. */

#include <assert.h>
#include <math.h>
#include "../common/tools.h"

static const float
two25 =  3.3554432000e+07f; /* 0x4c000000 */

float frexpf(float x, int *eptr)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_onef;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    int _xexp = 0;
    int32_t hx, ix;

    assert(eptr != (void*)0);
    if(eptr == (void*)0) {
        eptr = &_xexp;
    }

    GET_FLOAT_WORD(hx, x);
    ix = 0x7fffffff & hx;
    *eptr = 0;

    if (!FLT_UWORD_IS_FINITE(ix) || FLT_UWORD_IS_ZERO(ix)) {
        return x + x;                        /* 0,inf,nan */
    }

    if (FLT_UWORD_IS_SUBNORMAL(ix)) {        /* subnormal */
        x *= two25;
        GET_FLOAT_WORD(hx, x);
        ix = hx & 0x7fffffff;
        *eptr = -25;
    }

    *eptr += (ix >> 23) - 126;
    hx = (hx & 0x807fffffU) | 0x3f000000U;
    SET_FLOAT_WORD(x, hx);
    return x;
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double frexp(double x, int *eptr)
{
    return (double) frexpf((float) x, eptr);
}

#endif /* #ifdef __LIBMCS_DOUBLE_IS_32BITS */
