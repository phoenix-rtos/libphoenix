/* SPDX-License-Identifier: GTDGmbH */
/* Copyright 2020-2025 by GTD GmbH. */

/**
 *
 * This family of functions returns a predefined ``NaN``.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     float nanf(const char *payload);
 *     double nan(const char *payload);
 *     long double nanl(const char *payload);
 *
 * Description
 * ===========
 *
 * ``nan`` produces a fixed ``qNaN`` regardless of input.
 *
 * The return ``qNaN`` is ``0x7FF80000000D067D`` as ``double`` (float: ``0x7FCF067D``).
 *
 * This procedure is not C standard compliant as the payload parameter is
 * ignored. If your platform/toolchain provides ``strtod`` as a workaround the
 * call ``strtod("NAN(char-sequence)", NULL);`` can be used as a replacement if
 * that functionality is wanted (same with ``strtof`` and ``strtold``).
 *
 * Returns
 * =======
 *
 * ``nan`` returns a ``qNaN``.
 *
 * Exceptions
 * ==========
 *
 * Does not raise exceptions.
 *
 */

#include <math.h>
#include "../common/tools.h"

#ifndef __LIBMCS_DOUBLE_IS_32BITS

double nan(const char *payload)
{
    (void)payload;

    double x;
    INSERT_WORDS(x,0x7FF80000,0x000D067D);
    return x;
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double nanl(const char *payload)
{
    return (long double) nan(payload);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
