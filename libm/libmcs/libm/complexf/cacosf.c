/* SPDX-License-Identifier: NetBSD */

#include <complex.h>
#include <math.h>

float complex cacosf(float complex z)
{
#ifdef __LIBMCS_FPU_DAZ
    z *= __volatile_onef;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    float complex w;
    float complex tmp0;
    float tmp1;

    tmp0 = casinf(z);
    tmp1 = (float)M_PI_2 - crealf(tmp0);
    w = CMPLXF(tmp1, -cimagf(tmp0));

    return w;
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double complex cacos(double complex z)
{
    return (double complex) cacosf((float complex) z);
}

#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
