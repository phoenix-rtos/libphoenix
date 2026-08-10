/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * POSIX implementation - semaphores
 *
 * Copyright 2026 Phoenix Systems
 * Author: Michal Lach
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _LIBPHOENIX_SYS_SEMAPHORE_H_
#define _LIBPHOENIX_SYS_SEMAPHORE_H_

#include <assert.h>
#include <limits.h>
#include <sys/ioctl.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SEMAPHORE_PATH "/dev/posix/sem/"
#define SEMCTL_PATH    "/dev/posix/semctl"

STATIC_ASSERT(SEM_VALUE_MAX >= _POSIX_SEM_VALUE_MAX, "SEM_VALUE_MAX shall be greater or equal to _POSIX_SEM_VALUE_MAX");
STATIC_ASSERT(SEM_NSEMS_MAX >= _POSIX_SEM_NSEMS_MAX, "SEM_NSEMS_MAX shall be greater or equal to _POSIX_SEM_NSEMS_MAX");

/* not required by POSIX, but ensures that
 * sem_getvalue() does not return an overflowed value.
 */
STATIC_ASSERT(SEM_VALUE_MAX <= INT_MAX, "SEM_VALUE_MAX shall be smaller or equal to INT_MAX");

#define SEM_UP           _IO('s', 0x1)
#define SEM_DOWN         _IO('s', 0x2)
#define SEM_DOWN_TRY     _IO('s', 0x3)
#define SEM_DOWN_TIMEOUT _IOW('s', 0x4, time_t)
#define SEM_GETVALUE     _IOR('s', 0x5, unsigned int)

#ifdef __cplusplus
}
#endif

#endif /* _LIBPHOENIX_SYS_SEMAPHORE_H_ */
