/* SPDX-License-Identifier: GTDGmbH */
/* Copyright 2020-2025 by GTD GmbH. */

/**
 *
 * This global variable is used to contain the sign of the integral within
 * :ref:`lgamma`. Each call to :ref:`lgamma` will update :math:`signgam`.
 *
 * Note that usage of :math:`signgam` is not thread-safe when calling
 * :ref:`lgamma` on multiple threads.
 *
 * The declaration of the macro :math:`signgam` is in ``math.h``.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     int signgam;
 *
 */

#include "math.h"

int __signgam;
