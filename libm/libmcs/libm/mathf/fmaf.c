/* SPDX-License-Identifier: GTDGmbH */
/* Copyright 2020-2025 by GTD GmbH. */

#include <math.h>

float fmaf(float x, float y, float z)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_onef;
    y *= __volatile_onef;
    z *= __volatile_onef;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    return x * y + z;
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double fma(double x, double y, double z)
{
    return (double) fmaf((float) x, (float) y, (float) z);
}

#endif /* #ifdef __LIBMCS_DOUBLE_IS_32BITS */
