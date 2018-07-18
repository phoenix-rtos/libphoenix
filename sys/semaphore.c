/*
 * Phoenix-RTOS
 *
 * Semaphores
 *
 * Copyright 2012, 2017, 2018 Phoenix Systems
 * Copyright 2006 Pawel Pisarczyk
 * Author: Pawel Pisarczyk, Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <sys/time.h>
#include <errno.h>
#include <sys/threads.h>
#include <time.h>
#include <sys/resource.h>


int semaphoreCreate(semaphore_t *s, unsigned int v)
{
	if (s == NULL)
		return -EINVAL;

	if (mutexCreate(&s->mutex) != EOK)
		return -ENOMEM;

	if (condCreate(&s->cond) != EOK)
		return -ENOMEM;

	s->v = v;

	return EOK;
}


int semaphoreDown(semaphore_t *s, time_t timeout)
{
	int err = EOK;
	time_t now, when = 0;

	if (s == NULL)
		return -EINVAL;

	if (timeout) {
		gettime(&now, NULL);
		when = now + timeout;
	}

	mutexLock(s->mutex);
	for (;;) {
		if (s->v > 0) {
			--s->v;
			break;
		}

		if ((err = condWait(s->cond, s->mutex, timeout)) == -ETIME)
			break;

		if (timeout) {
			gettime(&now, NULL);

			if (now >= when)
				timeout = 1;
			else
				timeout = when - now;
		}
	}
	mutexUnlock(s->mutex);

	return err;
}


int semaphoreUp(semaphore_t *s)
{
	if (s == NULL)
		return -EINVAL;

	mutexLock(s->mutex);

	condSignal(s->cond);

	if (s->v == (unsigned int)-1) {
		mutexUnlock(s->mutex);
		return -EAGAIN;
	}

	++s->v;
	mutexUnlock(s->mutex);

	return EOK;
}


int semaphoreDone(semaphore_t *s)
{
	if (s == NULL)
		return -EINVAL;

	resourceDestroy(s->mutex);
	resourceDestroy(s->cond);

	return EOK;
}
