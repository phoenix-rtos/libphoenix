/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */

#include <math.h>
#include "../common/tools.h"

long int lroundf(float x)
{
    int32_t exponent_less_127;
    uint32_t w;
    long int result;
    int32_t sign;

    GET_FLOAT_WORD(w, x);
    exponent_less_127 = ((w & 0x7f800000) >> 23) - 127;
    sign = (w & 0x80000000U) != 0 ? -1 : 1;
    w &= 0x7fffff;
    w |= 0x800000;

    if (exponent_less_127 < (int32_t)((8 * sizeof(long int)) - 1)) {
        if (exponent_less_127 < 0) {
            return exponent_less_127 < -1 ? 0 : sign;
        } else if (exponent_less_127 >= 23) {
            result = (long int) w << (exponent_less_127 - 23);
        } else {
            w += 0x400000 >> exponent_less_127;
            result = w >> (23 - exponent_less_127);
        }
    } else {
        (void) __raise_invalidf();
        if (sign == -1) {
            return __MIN_LONG;
        } else {
            return __MAX_LONG;
        }
    }

    return sign * result;
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

long int lround(double x)
{
    return lroundf((float) x);
}

#endif /* #ifdef __LIBMCS_DOUBLE_IS_32BITS */
