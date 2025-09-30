/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */
/* Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com. */

#include <math.h>
#include "../common/tools.h"

#if INT_MAX > 50000
    #define OVERFLOW_INT 50000
#else
    #define OVERFLOW_INT 30000
#endif

static const float
two25   =  3.355443200e+07f,     /* 0x4c000000 */
twom25  =  2.9802322388e-08f;    /* 0x33000000 */

float scalbnf(float x, int n)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_onef;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    int32_t  k, ix;
    uint32_t hx;

    GET_FLOAT_WORD(ix, x);
    hx = ix & 0x7fffffff;
    k = hx >> 23;      /* extract exponent */

    if (FLT_UWORD_IS_ZERO(hx)) {
        return x;
    }

    if (!FLT_UWORD_IS_FINITE(hx)) {
        return x + x;    /* NaN or Inf */
    }

    if (FLT_UWORD_IS_SUBNORMAL(hx)) {
        x *= two25;
        GET_FLOAT_WORD(ix, x);
        k = ((ix & 0x7f800000) >> 23) - 25;

        if (n < -50000) {
            return __raise_underflowf(x);    /*underflow*/
        }
    }

    if (n > OVERFLOW_INT) {
        return __raise_overflowf(x);         /*overflow */
    }

    k = k + n;

    if (k > FLT_LARGEST_EXP) {
        return __raise_overflowf(x);         /*overflow */
    }

    if (k > 0) {               /* normal result */
        SET_FLOAT_WORD(x, (ix & 0x807fffffU) | (k << 23U));
        return x;
    }

    if (k < FLT_SMALLEST_EXP) {
        return __raise_underflowf(x);        /*underflow*/
    }

    k += 25;                /* subnormal result */
    SET_FLOAT_WORD(x, (ix & 0x807fffffU) | (k << 23U));
    return x * twom25;
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double scalbn(double x, int n)
{
    return (double) scalbnf((float) x, n);
}

#endif /* #ifdef __LIBMCS_DOUBLE_IS_32BITS */
