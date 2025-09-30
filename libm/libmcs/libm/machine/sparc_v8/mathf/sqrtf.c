/* SPDX-License-Identifier: GTDGmbH */
/* Copyright 2017-2022 by GTD GmbH. */

#include <math.h>

float sqrtf(float x)
{
    float root = 0.0f;

    __asm__ volatile ("fsqrts %[x], %[root]"
                      : [root] "=f" (root)
                      : [x] "f" (x));

    return root;
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double sqrt(double x)
{
    return (double) sqrtf((float) x);
}

#endif /* #ifdef __LIBMCS_DOUBLE_IS_32BITS */
