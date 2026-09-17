/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * setjmp.c
 *
 * Copyright 2026 Phoenix Systems
 * Author: Adam Greloch
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <setjmp.h>


__EXPORT_INLINE void longjmp(jmp_buf var, int m);
__EXPORT_INLINE void siglongjmp(sigjmp_buf env, int val);
