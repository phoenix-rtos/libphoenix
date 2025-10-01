/* SPDX-License-Identifier: NetBSD */

#include <complex.h>

float complex casinhf(float complex z)
{
#ifdef __LIBMCS_FPU_DAZ
    z *= __volatile_onef;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    float complex w;
    float complex tmp;

    /* w = -1.0f * I * casinf(z * I); */
    tmp = CMPLXF(-cimagf(z), crealf(z));
    tmp = casinf(tmp);
    w = CMPLXF(cimagf(tmp), -crealf(tmp));
    return w;
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double complex casinh(double complex z)
{
    return (double complex) casinhf((float complex) z);
}

#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
