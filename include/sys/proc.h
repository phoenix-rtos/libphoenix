/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/proc.h
 *
 * Copyright 2026 Phoenix Systems
 * Author: Adam Greloch
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _LIBPHOENIX_SYS_PROC_H_
#define _LIBPHOENIX_SYS_PROC_H_

#include <sys/types.h>
#include <phoenix/posix-proc.h>

#ifdef __cplusplus
extern "C" {
#endif


/*
 * Tells whether a process matching all of the given filters exists. Returns 1
 * if true, 0 if false, -1 with errno set on error. Zero-argument matches any
 * value, at least one of pid/pgid/sid must be non-zero.
 */
int procExists(pid_t pid, pid_t pgid, pid_t sid, unsigned int flags);


#ifdef __cplusplus
}
#endif

#endif
