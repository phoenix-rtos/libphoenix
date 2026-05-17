/* SPDX-License-Identifier: GTDGmbH */
/* Copyright 2020-2025 by GTD GmbH. */

#ifndef LIBMCS_TRIGD_H
#define LIBMCS_TRIGD_H

#include "../../common/tools.h"

extern double __sin(double x, double y, int iy);
extern double __cos(double x, double y);
extern int32_t __rem_pio2(double x, double *y);

#endif /* !LIBMCS_TRIGD_H */
