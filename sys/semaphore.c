/*
 * Phoenix-RTOS
 *
 * Semaphores
 *
 * Copyright 2012, 2017, 2018, 2026 Phoenix Systems
 * Copyright 2006 Pawel Pisarczyk
 * Author: Pawel Pisarczyk, Aleksander Kaminski, Michal Lach
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <assert.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/threads.h>
#include <sys/semaphore.h>
#include <time.h>
#include <sys/resource.h>


int semaphoreCount(semaphore_t *s)
{
	int ret = 0, count = 0;

	if (s == NULL) {
		return -EINVAL;
	}

	ret = mutexTry(s->mutex);
	if (ret != EOK) {
		return 0;
	}

	count = s->v;
	assert(count >= 0); /* s->v is unsigned, we don't want to overflow */
	mutexUnlock(s->mutex);

	return count;
}


int semaphoreCreate(semaphore_t *s, unsigned int v)
{
	if (s == NULL || v > SEM_VALUE_MAX) {
		return -EINVAL;
	}

	if (mutexCreate(&s->mutex) != EOK) {
		return -ENOMEM;
	}

	if (condCreate(&s->cond) != EOK) {
		return -ENOMEM;
	}

	s->v = v;

	return EOK;
}


int semaphoreDown(semaphore_t *s, time_t timeout)
{
	int err = EOK;
	time_t now, when = 0;

	if (s == NULL) {
		return -EINVAL;
	}

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

		if ((err = condWait(s->cond, s->mutex, timeout)) == -ETIME) {
			break;
		}

		if (timeout) {
			gettime(&now, NULL);

			if (now >= when) {
				timeout = 1;
			}
			else {
				timeout = when - now;
			}
		}
	}
	mutexUnlock(s->mutex);

	return err;
}


int semaphoreTryDown(semaphore_t *s)
{
	int ret = EOK;

	if (s == NULL) {
		return -EINVAL;
	}

	ret = mutexTry(s->mutex);
	if (ret != EOK) {
		return ret;
	}

	else if (s->v <= 0) {
		ret = -EAGAIN;
	}
	else {
		--s->v;
	}

	mutexUnlock(s->mutex);
	return ret;
}


int semaphoreUp(semaphore_t *s)
{
	int ret = EOK;

	if (s == NULL) {
		return -EINVAL;
	}

	mutexLock(s->mutex);
	condSignal(s->cond);

	if (s->v == (unsigned int)-1) {
		ret = -EAGAIN;
	}
	else if (s->v + 1 > SEM_VALUE_MAX) {
		ret = -EOVERFLOW;
	}
	else {
		++s->v;
	}

	mutexUnlock(s->mutex);
	return ret;
}


int semaphoreDone(semaphore_t *s)
{
	if (s == NULL) {
		return -EINVAL;
	}

	resourceDestroy(s->mutex);
	resourceDestroy(s->cond);

	return EOK;
}
