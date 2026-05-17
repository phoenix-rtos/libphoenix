/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */
/* Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com. */

#include <math.h>

float exp2f(float x)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_onef;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    return powf(2.0f, x);
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double exp2(double x)
{
    return (double) exp2f((float) x);
}

#endif /* #ifdef __LIBMCS_DOUBLE_IS_32BITS */
