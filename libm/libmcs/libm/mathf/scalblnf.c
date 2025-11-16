/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */
/* Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com. */

#include <math.h>
#include "../common/tools.h"

static const float
two25   =  3.355443200e+07f,    /* 0x4c000000 */
twom25  =  2.9802322388e-08f;    /* 0x33000000 */

float scalblnf(float x, long int n)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_onef;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    int32_t k, ix;
    GET_FLOAT_WORD(ix, x);
    k = (ix & 0x7f800000) >> 23;    /* extract exponent */

    if (k == 0) {              /* 0 or subnormal x */
        if ((ix & 0x7fffffff) == 0) {
            return x;    /* +-0 */
        }

        x *= two25;
        GET_FLOAT_WORD(ix, x);
        k = ((ix & 0x7f800000) >> 23) - 25;
    }

    if (k == 0xff) {
        return x + x;    /* NaN or Inf */
    }

    if (n > 50000) {
        return __raise_overflowf(x);     /*overflow*/
    }

    k = k + n;

    if (k >  0xfe) {
        return __raise_overflowf(x);     /*overflow*/
    }

    if (n < -50000) {
        return __raise_underflowf(x);    /*underflow*/
    }

    if (k > 0) {               /* normal result */
        SET_FLOAT_WORD(x, (ix & 0x807fffffU) | (k << 23U));
        return x;
    }

    if (k <= -25) {
        return __raise_underflowf(x);    /*underflow*/
    }

    k += 25;                /* subnormal result */
    SET_FLOAT_WORD(x, (ix & 0x807fffffU) | (k << 23U));
    return x * twom25;
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double scalbln(double x, long int n)
{
    return (double) scalblnf((float) x, n);
}

#endif /* #ifdef __LIBMCS_DOUBLE_IS_32BITS */
