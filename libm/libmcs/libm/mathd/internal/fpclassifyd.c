/* SPDX-License-Identifier: RedHat */
/* Copyright (C) 2002,2007 by  Red Hat, Incorporated. All rights reserved. */

/**
 *
 * This macro is used to classify :math:`x`. It can be called with ``float``,
 * ``double`` or ``long double`` input. This macro is implemented in
 * ``math.h``.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     int fpclassify(x);
 *
 * Description
 * ===========
 *
 * ``fpclassify`` returns different constants depending on whether the input is
 * subnormal, normal, zero, infinite or :math:`NaN`.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    fpclassify(x) = \left\{\begin{array}{ll} FP\_ZERO, & x = \pm 0.0 \\
 *                                             FP\_SUBNORMAL, & x \in \mathbb{S} \\
 *                                             FP\_INFINITE, & x = \pm Inf \\
 *                                             FP\_NAN, & x = NaN \\
 *                                             FP\_NORMAL, & otherwise \end{array}\right.
 *
 * Returns
 * =======
 *
 * ``fpclassify`` returns constant values, see ``math.h`` for their specific values.
 *
 * Exceptions
 * ==========
 *
 * Does not raise exceptions.
 *
 * Output map
 * ==========
 *
 * +---------------------+-----------------+------------------------------+------------------------------+-------------+------------------------------+------------------------------+-----------------+-------------+
 * | **x**               | :math:`-Inf`    | :math:`<0 \notin \mathbb{S}` | :math:`<0 \in \mathbb{S}`    | :math:`±0`  | :math:`>0 \in \mathbb{S}`    | :math:`>0 \notin \mathbb{S}` | :math:`+Inf`    | :math:`NaN` |
 * +=====================+=================+==============================+==============================+=============+==============================+==============================+=================+=============+
 * | **fpclassify(x)**   | ``FP_INFINITE`` | ``FP_NORMAL``                | ``FP_SUBNORMAL``             | ``FP_ZERO`` | ``FP_SUBNORMAL``             | ``FP_NORMAL``                | ``FP_INFINITE`` | ``FP_NAN``  |
 * +---------------------+-----------------+------------------------------+------------------------------+-------------+------------------------------+------------------------------+-----------------+-------------+
 *
 */

#include <math.h>
#include "../../common/tools.h"

int __fpclassifyd(double x)
{
    uint32_t msw, lsw;

    EXTRACT_WORDS(msw, lsw, x);
    msw &= 0x7fffffffU;

    if (msw == 0x00000000U && lsw == 0x00000000U) {
        return FP_ZERO;
    } else if (msw >= 0x00100000U && msw <= 0x7fefffffU) {
        return FP_NORMAL;
    } else if (msw <= 0x000fffffU) {    /* zero is already handled above */
        return FP_SUBNORMAL;
    } else if (msw == 0x7ff00000U && lsw == 0x00000000U) {
        return FP_INFINITE;
    } else {
        return FP_NAN;
    }
}
