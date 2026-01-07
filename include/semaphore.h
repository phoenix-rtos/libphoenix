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

#include <time.h>
#include <limits.h>
#include <sys/threads.h>
#include <sys/semaphore.h>

#define SEM_FAILED ((sem_t *)0xDAAB0000)

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
