/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */
/* lrint adapted to be llrint for Newlib, 2009 by Craig Howland. */

/*
 * llrintf(x)
 * Return x rounded to integral value according to the prevailing
 * rounding mode.
 * Method:
 *    Using floating addition.
 * Exception:
 *    Inexact flag raised if x not equal to llrintf(x).
 */

#include <math.h>
#include "../common/tools.h"

static const float
/* Adding a float, x, to 2^23 will cause the result to be rounded based on
   the fractional part of x, according to the implementation's current rounding
   mode.  2^23 is the smallest float that can be represented using all 23 significant
   digits. */
TWO23[2] = {
     8.3886080000e+06f, /* 0x4b000000 */
    -8.3886080000e+06f, /* 0xcb000000 */
};

long long int llrintf(float x)
{
    int32_t _j0, sx;
    uint32_t _i0;
    float t;
    volatile float w;
    long long int result;

    GET_FLOAT_WORD(_i0, x);

    /* Extract sign bit. */
    sx = (_i0 >> 31);

    /* Extract exponent field. */
    _j0 = ((_i0 & 0x7f800000) >> 23) - 127;

    if (_j0 < (int32_t)(sizeof(long long int) * 8) - 1) {
        if (_j0 < -1) {
            return 0;
        } else if (_j0 >= 23) {
            result = (long long int)((_i0 & 0x7fffff) | 0x800000) << (_j0 - 23);
        } else {
            w = TWO23[sx] + x;
            t = w - TWO23[sx];
            GET_FLOAT_WORD(_i0, t);

            /* Detect the all-zeros representation of plus and
               minus zero, which fails the calculation below. */
            if ((_i0 & ~((uint32_t)1 << 31)) == 0) {
                return 0;
            }

            _j0 = ((_i0 >> 23) & 0xff) - 0x7f;
            _i0 &= 0x7fffff;
            _i0 |= 0x800000;
            result = _i0 >> (23 - _j0);
        }
    } else {
        (void) __raise_invalidf();
        if (sx != 0) {
            return __MIN_LONG_LONG;
        }
        else {
            return __MAX_LONG_LONG;
        }
    }

    return (sx != 0) ? -result : result;
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

long long int llrint(double x)
{
    return llrintf((float) x);
}

#endif /* #ifdef __LIBMCS_DOUBLE_IS_32BITS */
