/* SPDX-License-Identifier: NetBSD */

#include <complex.h>

float complex casinf(float complex z)
{
#ifdef __LIBMCS_FPU_DAZ
    z *= __volatile_onef;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    float complex w;
    float complex ct, zz, z2;
    float x, y;

    x = crealf(z);
    y = cimagf(z);

    ct = CMPLXF(-y, x);
    /* zz = (x - y) * (x + y) + (2.0f * x * y) * I; */
    zz = CMPLXF((x - y) * (x + y), 2.0f * x * y);

    /* zz = 1.0f - crealf(zz) - cimagf(zz) * I; */
    zz = CMPLXF(1.0f - crealf(zz), -cimagf(zz));
    z2 = csqrtf(zz);

    zz = ct + z2;
    zz = clogf(zz);
    /* w = zz * (-1.0f * I); */
    w = CMPLXF(cimagf(zz), -crealf(zz));
    return w;
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double complex casin(double complex z)
{
    return (double complex) casinf((float complex) z);
}

#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
