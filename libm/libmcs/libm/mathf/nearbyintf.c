/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */

#include <math.h>

float nearbyintf(float x)
{
    return rintf(x);
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double nearbyint(double x)
{
    return (double) nearbyintf((float) x);
}

#endif /* #ifdef __LIBMCS_DOUBLE_IS_32BITS */
