/* SPDX-License-Identifier: NetBSD */

#include <complex.h>
#include <math.h>

float complex ctanhf(float complex z)
{
#ifdef __LIBMCS_FPU_DAZ
    z *= __volatile_onef;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    float complex w;
    float x, y, d;

    x = crealf(z);
    y = cimagf(z);
    d = coshf(2.0f * x) + cosf(2.0f * y);
    /* w = sinhf(2.0f * x) / d  + (sinf(2.0f * y) / d) * I; */
    w = CMPLXF(sinhf(2.0f * x) / d, sinf(2.0f * y) / d);

    return w;
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double complex ctanh(double complex z)
{
    return (double complex) ctanhf((float complex) z);
}

#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
