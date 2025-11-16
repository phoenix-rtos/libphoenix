/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */

#ifndef LIBMCS_LOG1PMFF_H
#define LIBMCS_LOG1PMFF_H

static const float
/* |(log(1+s)-log(1-s))/s - Lg(s)| < 2**-34.24 (~[-4.95e-11, 4.97e-11]). */
Lg1 =      0xaaaaaa.0p-24f, /* 0.66666662693, 0x3F2AAAAA */
Lg2 =      0xccce13.0p-25f, /* 0.40000972152, 0x3ECCCE13 */
Lg3 =      0x91e9ee.0p-25f, /* 0.28498786688, 0x3E91E9EE */
Lg4 =      0xf89e26.0p-26f; /* 0.24279078841, 0x3E789E26 */

static inline float __log1pmff(float f)
{
    float hfsq, s, z, R, w, t1, t2;

    s = f / (2.0f + f);
    z = s * s;
    w = z * z;
    t1 = w * (Lg2 + w * Lg4);
    t2 = z * (Lg1 + w * Lg3);
    R = t2 + t1;
    hfsq = 0.5f * f * f;
    return s * (hfsq + R);
}

#endif /* !LIBMCS_LOG1PMFF_H */
