/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * POSIX implementation - semaphores
 *
 * Copyright 2026 Phoenix Systems
 * Author: Michał Lach
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <limits.h>
#include <sys/ioctl.h>

#define SEMAPHORE_PATH "/dev/posix/sem/"
#define SEMCTL_PATH    "/dev/posix/semctl"

STATIC_ASSERT(SEM_VALUE_MAX >= _POSIX_SEM_VALUE_MAX, "SEM_VALUE_MAX shall be greater or equal to _POSIX_SEM_VALUE_MAX");
STATIC_ASSERT(SEM_NSEMS_MAX >= _POSIX_SEM_NSEMS_MAX, "SEM_NSEMS_MAX shall be greater or equal to _POSIX_SEM_NSEMS_MAX");

#define SEM_UP           _IO('s', 0x1)
#define SEM_DOWN         _IO('s', 0x2)
#define SEM_DOWN_TRY     _IO('s', 0x3)
#define SEM_DOWN_TIMEOUT _IOW('s', 0x4, time_t)
