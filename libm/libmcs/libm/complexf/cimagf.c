/* SPDX-License-Identifier: PublicDomain */
/* Written by Matthias Drochner <drochner@NetBSD.org>. */

#include <complex.h>
#include "../common/tools.h"

float cimagf(float complex z)
{
    float_complex w;
    w.z = z;

    return (IMAG_PART(w));
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double cimag(double complex z)
{
    return (double) cimagf((float complex) z);
}

#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
