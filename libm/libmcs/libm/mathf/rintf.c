/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */

#include <math.h>
#include "../common/tools.h"

static const float
TWO23[2] = {
     8.3886080000e+06f, /* 0x4b000000 */
    -8.3886080000e+06f, /* 0xcb000000 */
};

float rintf(float x)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_onef;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    int32_t _i0, _j0, sx;
    uint32_t i, _i1, ix;
    float t;
    volatile float w;

    GET_FLOAT_WORD(_i0, x);
    sx = (_i0 >> 31) & 1;
    ix = (_i0 & 0x7fffffff);
    _j0 = (ix >> 23) - 0x7f;

    if (_j0 < 23) {
        if (FLT_UWORD_IS_ZERO(ix)) {
            return x;
        }

        if (_j0 < 0) {
            _i1 = (_i0 & 0x07fffff);
            _i0 &= 0xfff00000U;
            _i0 |= ((_i1 | -_i1) >> 9) & 0x400000;
            SET_FLOAT_WORD(x, _i0);
            w = TWO23[sx] + x;
            t =  w - TWO23[sx];
            GET_FLOAT_WORD(_i0, t);
            SET_FLOAT_WORD(t, (_i0 & 0x7fffffff) | (sx << 31));
            return t;
        } else {
            i = (0x007fffff) >> _j0;

            if ((_i0 & i) == 0) {
                return x;    /* x is integral */
            }

            i >>= 1;

            if ((_i0 & i) != 0) {
                _i0 = (_i0 & (~i)) | ((0x200000) >> _j0);
            }
        }
    } else {
        if (!FLT_UWORD_IS_FINITE(ix)) {
            return x + x;    /* inf or NaN */
        } else {
            return x;    /* x is integral */
        }
    }

    SET_FLOAT_WORD(x, _i0);
    w = TWO23[sx] + x;
    return w - TWO23[sx];
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double rint(double x)
{
    return (double) rintf((float) x);
}

#endif /* #ifdef __LIBMCS_DOUBLE_IS_32BITS */
