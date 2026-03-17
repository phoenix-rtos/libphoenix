/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */
/* Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com. */

#include <math.h>
#include "../common/tools.h"

static const float atanhi[] = {
    4.6364760399e-01f, /* atan(0.5)hi 0x3eed6338 */
    7.8539812565e-01f, /* atan(1.0)hi 0x3f490fda */
    9.8279368877e-01f, /* atan(1.5)hi 0x3f7b985e */
    1.5707962513e+00f, /* atan(inf)hi 0x3fc90fda */
};

static const float atanlo[] = {
    5.0121582440e-09f, /* atan(0.5)lo 0x31ac3769 */
    3.7748947079e-08f, /* atan(1.0)lo 0x33222168 */
    3.4473217170e-08f, /* atan(1.5)lo 0x33140fb4 */
    7.5497894159e-08f, /* atan(inf)lo 0x33a22168 */
};

static const float aT[] = {
     3.3333334327e-01f, /* 0x3eaaaaaa */
    -2.0000000298e-01f, /* 0xbe4ccccd */
     1.4285714924e-01f, /* 0x3e124925 */
    -1.1111110449e-01f, /* 0xbde38e38 */
     9.0908870101e-02f, /* 0x3dba2e6e */
    -7.6918758452e-02f, /* 0xbd9d8795 */
     6.6610731184e-02f, /* 0x3d886b35 */
    -5.8335702866e-02f, /* 0xbd6ef16b */
     4.9768779427e-02f, /* 0x3d4bda59 */
    -3.6531571299e-02f, /* 0xbd15a221 */
     1.6285819933e-02f, /* 0x3c8569d7 */
};

static const float one = 1.0f;

float atanf(float x)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_onef;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    float w, s1, s2, z;
    int32_t ix, hx, id;

    GET_FLOAT_WORD(hx, x);
    ix = hx & 0x7fffffff;

    if (ix >= 0x50800000) { /* if |x| >= 2^34 */
        if (FLT_UWORD_IS_NAN(ix)) {
            return x + x;    /* NaN */
        }

        if (hx > 0) {
            return  atanhi[3] + atanlo[3];
        } else {
            return -atanhi[3] - atanlo[3];
        }
    }

    if (ix < 0x3ee00000) {    /* |x| < 0.4375 */
        if (ix < 0x31000000) {    /* |x| < 2^-29 */
            if (FLT_UWORD_IS_ZERO(ix)) {    /* return x inexact except 0 */
                return x;
            } else {
                return __raise_inexactf(x);
            }
        }

        id = -1;
    } else {
        x = fabsf(x);

        if (ix < 0x3f980000) {        /* |x| < 1.1875 */
            if (ix < 0x3f300000) {    /* 7/16 <=|x|<11/16 */
                id = 0;
                x = (2.0f * x - one) / (2.0f + x);
            } else {            /* 11/16<=|x|< 19/16 */
                id = 1;
                x  = (x - one) / (x + one);
            }
        } else {
            if (ix < 0x401c0000) {    /* |x| < 2.4375 */
                id = 2;
                x  = (x - 1.5f) / (one + 1.5f * x);
            } else {            /* 2.4375 <= |x| < 2^66 */
                id = 3;
                x  = -1.0f / x;
            }
        }
    }

    /* end of argument reduction */
    z = x * x;
    w = z * z;
    /* break sum from i=0 to 10 aT[i]z**(i+1) into odd and even poly */
    s1 = z * (aT[0] + w * (aT[2] + w * (aT[4] + w * (aT[6] + w * (aT[8] + w * aT[10])))));
    s2 = w * (aT[1] + w * (aT[3] + w * (aT[5] + w * (aT[7] + w * aT[9]))));

    if (id < 0) {
        return x - x * (s1 + s2);
    } else {
        z = atanhi[id] - ((x * (s1 + s2) - atanlo[id]) - x);
        return (hx < 0) ? -z : z;
    }
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double atan(double x)
{
    return (double) atanf((float) x);
}

#endif /* #ifdef __LIBMCS_DOUBLE_IS_32BITS */
