/* SPDX-License-Identifier: GTDGmbH */
/* Copyright 2020-2025 by GTD GmbH. */

#ifndef LIBMCS_TRIGF_H
#define LIBMCS_TRIGF_H

#include "../../common/tools.h"

extern float __sinf(float x, float y, int iy);
extern float __cosf(float x, float y);
extern int32_t __rem_pio2f(float x, float *y);

#endif /* !LIBMCS_TRIGF_H */
