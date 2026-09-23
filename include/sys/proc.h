/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/proc.h
 *
 * Extensions to POSIX layer for group/session/ctty management.
 *
 * Copyright 2026 Phoenix Systems
 * Author: Adam Greloch, Ziemowit Leszczynski
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


/*
 * Records whether a session holds a controlling terminal, so that the kernel
 * can enforce POSIX's "at most one per session" rule for terminal
 * drivers, which cannot see each other's state.
 *
 * Called by a terminal driver on behalf of the session leader that issued
 * TIOCSCTTY (acquire != 0) or TIOCNOTTY (acquire == 0), hence the explicit
 * sid. Acquiring for a session that already holds one fails with EPERM;
 * acquiring for anything but a live session leader fails with ESRCH/EPERM.
 * Not meant to be called by applications.
 */
int sessionCtty(pid_t sid, int acquire);


#ifdef __cplusplus
}
#endif

#endif
