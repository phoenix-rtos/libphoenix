/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */
/* Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com. */

#ifndef LIBMCS_ERRORFUNCTIONF_H
#define LIBMCS_ERRORFUNCTIONF_H

#include <math.h>
#include "../../common/tools.h"

static const float
one  =  1.00000000e+00f, /* 0x3F800000 */
/*
 * Domain [0, 0.84375], range ~[-5.4419e-10, 5.5179e-10]:
 * |(erf(x) - x)/x - pp(x)/qq(x)| < 2**-31
 */
pp0  =  1.28379166e-01f, /* 0x3e0375d4 */
pp1  = -3.36030394e-01f, /* 0xbeac0c2d */
pp2  = -1.86261395e-03f, /* 0xbaf422f4 */
qq1  =  3.12324315e-01f, /* 0x3e9fe8f9 */
qq2  =  2.16070414e-02f, /* 0x3cb10140 */
qq3  = -1.98859372e-03f, /* 0xbb025311 */
/*
 * Domain [0.84375, 1.25], range ~[-1.023e-9, 1.023e-9]:
 * |(erf(x) - erx) - pa(x)/qa(x)| < 2**-31
 */
pa0  =  3.65041046e-06f, /* 0x3674f993 */
pa1  =  4.15109307e-01f, /* 0x3ed48935 */
pa2  = -2.09395722e-01f, /* 0xbe566bd5 */
pa3  =  8.67677554e-02f, /* 0x3db1b34b */
qa1  =  4.95560974e-01f, /* 0x3efdba2b */
qa2  =  3.71248513e-01f, /* 0x3ebe1449 */
qa3  =  3.92478965e-02f, /* 0x3d20c267 */
/*
 * Domain [1.25,1/0.35], range ~[-4.821e-9, 4.927e-9]:
 * |log(x*erfc(x)) + x**2 + 0.5625 - ra(x)/sa(x)| < 2**-28
 */
ra0  = -9.88156721e-03f, /* 0xbc21e64c */
ra1  = -5.43658376e-01f, /* 0xbf0b2d32 */
ra2  = -1.66828310e+00f, /* 0xbfd58a4d */
ra3  = -6.91554189e-01f, /* 0xbf3109b2 */
sa1  =  4.48581553e+00f, /* 0x408f8bcd */
sa2  =  4.10799170e+00f, /* 0x408374ab */
sa3  =  5.53855181e-01f, /* 0x3f0dc974 */
/*
 * Domain [2.85715, 11], range ~[-1.484e-9, 1.505e-9]:
 * |log(x*erfc(x)) + x**2 + 0.5625 - rb(x)/sb(x)| < 2**-30
 */
rb0  = -9.86496918e-03f, /* 0xbc21a0ae */
rb1  = -5.48049808e-01f, /* 0xbf0c4cfe */
rb2  = -1.84115684e+00f, /* 0xbfebab07 */
sb1  =  4.87132740e+00f, /* 0x409be1ea */
sb2  =  3.04982710e+00f, /* 0x4043305e */
sb3  = -7.61900663e-01f; /* 0xbf430bec */

static inline float __erff_y(float x)
{
    float s, z, r;
    z = x * x;
    r = pp0 + z * (pp1 + z * pp2);
    s = one + z * (qq1 + z * (qq2 + z * qq3));
    return r / s;
}

static inline float __erff_P(float s)
{
    return pa0 + s * (pa1 + s * (pa2 + s * pa3));
}

static inline float __erff_Q(float s)
{
    return one + s * (qa1 + s * (qa2 + s * qa3));
}

static inline float __erff_Ra(float s)
{
    return ra0 + s * (ra1 + s * (ra2 + s * ra3));
}

static inline float __erff_Sa(float s)
{
    return one + s * (sa1 + s * (sa2 + s * sa3));
}

static inline float __erff_Rb(float s)
{
    return rb0 + s * (rb1 + s * rb2);
}

static inline float __erff_Sb(float s)
{
    return one + s * (sb1 + s * (sb2 + s * sb3));
}

#endif /* !LIBMCS_ERRORFUNCTIONF_H */
