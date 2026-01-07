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
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _LIBPHOENIX_SEMAPHORE_H_
#define _LIBPHOENIX_SEMAPHORE_H_

#include <time.h>
#include <limits.h>
#include <sys/threads.h>
#include <sys/semaphore.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SEM_FAILED ((sem_t *)0)


typedef struct _sem_t {
	/* clang-format off */
	enum { smNamed, smUnnamed } type;
	/* clang-format on */

	union {
		semaphore_t unnamed;
		oid_t named;
	};
} sem_t;


extern int sem_wait(sem_t *sem);


extern int sem_trywait(sem_t *sem);


extern int sem_timedwait(sem_t *restrict sem, const struct timespec *restrict abs_timeout);


extern int sem_getvalue(sem_t *restrict sem, int *restrict value);


extern int sem_post(sem_t *sem);


extern int sem_close(sem_t *sem);


extern sem_t *sem_open(const char *name, int oflag, ... /* mode_t mode, unsigned int value */);


extern int sem_unlink(const char *name);


extern int sem_destroy(sem_t *sem);


extern int sem_init(sem_t *sem, int pshared, unsigned int value);


#ifdef __cplusplus
}
#endif


#endif /* _LIBPHOENIX_SEMAPHORE_H_ */
