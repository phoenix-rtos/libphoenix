/* SPDX-License-Identifier: GTDGmbH */
/* Copyright 2020-2025 by GTD GmbH. */

#include <math.h>
#include "../common/tools.h"

float nanf(const char *payload)
{
    (void)payload;

    float x;
    SET_FLOAT_WORD(x,0x7FCF067D);
    return x;
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double nan(const char *payload)
{
    return (double) nanf(payload);
}

#endif /* #ifdef __LIBMCS_DOUBLE_IS_32BITS */
