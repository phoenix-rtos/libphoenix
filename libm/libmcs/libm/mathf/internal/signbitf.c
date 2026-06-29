/* SPDX-License-Identifier: RedHat */
/* Copyright (C) 2002 by  Red Hat, Incorporated. All rights reserved. */

/*
FUNCTION
<<signbit>>---Does floating-point number have negative sign?

INDEX
    signbit

SYNOPSIS
    #include <math.h>
    int signbit(real-floating <[x]>);

DESCRIPTION
The <<signbit>> macro determines whether the sign of its argument value is
negative.  The macro reports the sign of all values, including infinities,
zeros, and NaNs.  If zero is unsigned, it is treated as positive.  As shown in
the synopsis, the argument is "real-floating," meaning that any of the real
floating-point types (float, double, etc.) may be given to it.

Note that because of the possibilities of signed 0 and NaNs, the expression
"<[x]> < 0.0" does not give the same result as <<signbit>> in all cases.

RETURNS
The <<signbit>> macro returns a nonzero value if and only if the sign of its
argument value is negative.

PORTABILITY
C99, POSIX.

*/

#include <math.h>
#include "../../common/tools.h"

int __signbitf(float x)
{
    uint32_t w;

    GET_FLOAT_WORD(w, x);

    return (w & 0x80000000U) != 0;
}
