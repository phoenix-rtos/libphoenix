/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */

/*
 * __log1pmf(f):
 * Return log(1+f) - f for 1+f in ~[sqrt(2)/2, sqrt(2)].
 */

#ifndef LIBMCS_LOG1PMFD_H
#define LIBMCS_LOG1PMFD_H

#ifndef __LIBMCS_DOUBLE_IS_32BITS

static const double
Lg1 = 6.666666666666735130e-01,  /* 0x3FE5555555555593 */
Lg2 = 3.999999999940941908e-01,  /* 0x3FD999999997FA04 */
Lg3 = 2.857142874366239149e-01,  /* 0x3FD2492494229359 */
Lg4 = 2.222219843214978396e-01,  /* 0x3FCC71C51D8E78AF */
Lg5 = 1.818357216161805012e-01,  /* 0x3FC7466496CB03DE */
Lg6 = 1.531383769920937332e-01,  /* 0x3FC39A09D078C69F */
Lg7 = 1.479819860511658591e-01;  /* 0x3FC2F112DF3E5244 */

/*
 * We always inline __log1pmf(), since doing so produces a substantial
 * performance improvement (~40% on amd64).
 */
static inline double __log1pmf(double f)
{
    double hfsq, s, z, R, w, t1, t2;

    s = f / (2.0 + f);
    z = s * s;
    w = z * z;
    t1 = w * (Lg2 + w * (Lg4 + w * Lg6));
    t2 = z * (Lg1 + w * (Lg3 + w * (Lg5 + w * Lg7)));
    R = t2 + t1;
    hfsq = 0.5 * f * f;
    return s * (hfsq + R);
}

#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */

#endif /* !LIBMCS_LOG1PMFD_H */
