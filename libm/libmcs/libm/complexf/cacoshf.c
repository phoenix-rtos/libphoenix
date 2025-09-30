/* SPDX-License-Identifier: NetBSD */

#include <complex.h>

float complex cacoshf(float complex z)
{
#ifdef __LIBMCS_FPU_DAZ
    z *= __volatile_onef;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    float complex w;

    w = clogf(z + csqrtf(z + 1) * csqrtf(z - 1));

    return w;
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double complex cacosh(double complex z)
{
    return (double complex) cacoshf((float complex) z);
}

#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
