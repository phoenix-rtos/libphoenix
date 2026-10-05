/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/proc.c
 *
 * Extensions to POSIX layer for group/session/ctty management.
 *
 * Copyright 2026 Phoenix Systems
 * Author: Adam Greloch
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <sys/proc.h>

__EXPORT_INLINE bool pidExists(pid_t pid);
__EXPORT_INLINE bool pgidExists(pid_t pgid);
