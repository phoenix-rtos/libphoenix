/* SPDX-License-Identifier: NetBSD */

#include <complex.h>
#include <math.h>
#include "internal/ctrigf.h"

float complex catanf(float complex z)
{
#ifdef __LIBMCS_FPU_DAZ
    z *= __volatile_onef;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    float complex w;
    float a, t, x, x2, y, tmp;

    x = crealf(z);
    y = cimagf(z);

    if ((x == 0.0f) && (y > 1.0f)) {
        goto ovrf;
    }

    x2 = x * x;
    a = 1.0f - x2 - (y * y);

    t = 0.5f * atan2f(2.0f * x, a);
    tmp = __redupif(t);

    t = y - 1.0f;
    a = x2 + (t * t);

    t = y + 1.0f;
    a = (x2 + (t * t)) / a;
    /* w = tmp + (0.25f * logf(a)) * I; */
    w = CMPLXF(tmp, 0.25f * logf(a));
    return w;

ovrf:
    w = CMPLXF(HUGE_VALF, HUGE_VALF);
    return w;
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double complex catan(double complex z)
{
    return (double complex) catanf((float complex) z);
}

#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
