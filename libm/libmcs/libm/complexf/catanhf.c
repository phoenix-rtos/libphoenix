/* SPDX-License-Identifier: NetBSD */

#include <complex.h>

float complex catanhf(float complex z)
{
#ifdef __LIBMCS_FPU_DAZ
    z *= __volatile_onef;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    float complex w;
    float complex tmp;

    /* w = -1.0f * I * catanf(z * I); */
    tmp = CMPLXF(-cimagf(z), crealf(z));
    tmp = catanf(tmp);
    w = CMPLXF(cimagf(tmp), -crealf(tmp));
    return w;
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double complex catanh(double complex z)
{
    return (double complex) catanhf((float complex) z);
}

#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
