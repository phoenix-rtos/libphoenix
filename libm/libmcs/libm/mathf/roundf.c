/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */

#include <math.h>
#include "../common/tools.h"

float roundf(float x)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_onef;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    uint32_t w;
    /* Most significant word, least significant word. */
    int32_t exponent_less_127;

    GET_FLOAT_WORD(w, x);

    /* Extract exponent field. */
    exponent_less_127 = (int32_t)((w & 0x7f800000) >> 23) - 127;

    if (exponent_less_127 < 23) {
        if (exponent_less_127 < 0) {
            w &= 0x80000000U;

            if (exponent_less_127 == -1) {
                /* Result is +1.0 or -1.0. */

                w |= ((uint32_t)127 << 23);
            }
        } else {
            uint32_t exponent_mask = 0x007fffff >> exponent_less_127;

            if ((w & exponent_mask) == 0) {
                /* x has an integral value. */

                return x;
            }

            w += 0x00400000 >> exponent_less_127;
            w &= ~exponent_mask;
        }
    } else {
        if (exponent_less_127 == 128) {
            /* x is NaN or infinite. */

            return x + x;
        } else {
            return x;
        }
    }

    SET_FLOAT_WORD(x, w);
    return x;
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double round(double x)
{
    return (double) roundf((float) x);
}

#endif /* #ifdef __LIBMCS_DOUBLE_IS_32BITS */
