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

#define SEMAPHORE_PATH      "/dev/posix/sem/"
#define SEMCTL_PATH         "/dev/posix/semctl"
#define SEMAPHORE_MAX_COUNT (128)
#define SEMAPHORE_NAME_MAX  (NAME_MAX - sizeof(SEMAPHORE_PATH) - 1)
#define SEM_VALUE_MAX       INT_MAX

#define SEM_UP           _IO('s', 0x1)
#define SEM_DOWN         _IO('s', 0x2)
#define SEM_DOWN_TRY     _IO('s', 0x3)
#define SEM_DOWN_TIMEOUT _IOW('s', 0x4, time_t)
