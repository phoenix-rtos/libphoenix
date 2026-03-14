/* SPDX-License-Identifier: NetBSD */

#include <complex.h>
#include <math.h>

float complex cexpf(float complex z)
{
#ifdef __LIBMCS_FPU_DAZ
    z *= __volatile_onef;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    float complex w;
    float r, x, y;

    x = crealf(z);
    y = cimagf(z);
    r = expf(x);
    /* w = r * cosf(y) + r * sinf(y) * I; */
    w = CMPLXF(r * cosf(y), r * sinf(y));
    return w;
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double complex cexp(double complex z)
{
    return (double complex) cexpf((float complex) z);
}

#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
