/* SPDX-License-Identifier: RedHat */
/* Copyright (C) 2002 by  Red Hat, Incorporated. All rights reserved. */

#include <math.h>
#include "../common/tools.h"

float fmaxf(float x, float y)
{
    if (isnan(x)) {
        if (__issignalingf(x) != 0 || __issignalingf(y) != 0) {
            return x * y;
        }
#ifdef __LIBMCS_FPU_DAZ
        y *= __volatile_onef;
#endif /* defined(__LIBMCS_FPU_DAZ) */
        return y;
    }

    if (isnan(y)) {
        if (__issignalingf(y) != 0) {
            return x * y;
        }
#ifdef __LIBMCS_FPU_DAZ
        x *= __volatile_onef;
#endif /* defined(__LIBMCS_FPU_DAZ) */
        return x;
    }
    
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_onef;
    y *= __volatile_onef;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    return x > y ? x : y;
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double fmax(double x, double y)
{
    return (double) fmaxf((float) x, (float) y);
}

#endif /* #ifdef __LIBMCS_DOUBLE_IS_32BITS */
