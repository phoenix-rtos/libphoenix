/* SPDX-License-Identifier: PublicDomain */
/* Written by Matthias Drochner <drochner@NetBSD.org>. */

#include <complex.h>
#include "../common/tools.h"

float complex conjf(float complex z)
{
    float_complex w = { .z = z };

    IMAG_PART(w) = -IMAG_PART(w);

    return (w.z);
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double complex conj(double complex z)
{
    return (double complex) conjf((float complex) z);
}

#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
