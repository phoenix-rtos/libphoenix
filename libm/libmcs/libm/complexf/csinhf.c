/* SPDX-License-Identifier: NetBSD */

#include <complex.h>
#include <math.h>

float complex csinhf(float complex z)
{
#ifdef __LIBMCS_FPU_DAZ
    z *= __volatile_onef;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    float complex w;
    float x, y;

    x = crealf(z);
    y = cimagf(z);
    /* w = sinhf(x) * cosf(y) + (coshf(x) * sinf(y)) * I; */
    w = CMPLXF(sinhf(x) * cosf(y), coshf(x) * sinf(y));
    return w;
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double complex csinh(double complex z)
{
    return (double complex) csinhf((float complex) z);
}

#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
