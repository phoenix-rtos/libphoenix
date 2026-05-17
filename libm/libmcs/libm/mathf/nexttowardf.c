/* SPDX-License-Identifier: RichFelker */
/* Copyright © 2005-2014 Rich Felker, et al. */

#include <math.h>
#include "../common/tools.h"

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

union fshape {
    float value;
    uint32_t bits;
};

float nexttowardf(float x, long double y)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_onef;
    y *= (long double)__volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    union fshape ux;
    uint32_t e;

    if (isnan(x) || isnan(y)) {
        return x + y;
    }

    if (x == y) {
        return y;
    }

    ux.value = x;

    if (x == 0) {
#ifdef __LIBMCS_FPU_DAZ
        ux.bits = 0x00800000U;  /* return +-minnormal */
#else
        ux.bits = 1U;           /* return +-minsubnormal */
#endif /* defined(__LIBMCS_FPU_DAZ) */

        if (signbit(y) != 0) {
            ux.bits |= 0x80000000U;
        }
    } else if (x < y) {
        if (signbit(x) != 0) {
            ux.bits--;
        } else {
            ux.bits++;
        }
    } else {
        if (signbit(x) != 0) {
            ux.bits++;
        } else {
            ux.bits--;
        }
    }

    e = ux.bits & 0x7f800000;

    /* raise overflow if ux.value is infinite and x is finite */
    if (e == 0x7f800000) {
        return __raise_overflowf(x);
    }

    /* raise underflow if ux.value is subnormal or zero */
    if (e == 0) {
#ifdef __LIBMCS_FPU_DAZ
        ux.bits = 0U;           /* return +-0.0 */

        if (signbit(x) != 0) {
            ux.bits |= 0x80000000U;
        }
#else
        (void) __raise_underflowf(x);
#endif /* defined(__LIBMCS_FPU_DAZ) */
    }

    return ux.value;
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double nexttoward(double x, long double y)
{
    return (double) nexttowardf((float) x, y);
}

#endif /* #ifdef __LIBMCS_DOUBLE_IS_32BITS */

#endif /* __LIBMCS_LONG_DOUBLE_IS_64BITS */
