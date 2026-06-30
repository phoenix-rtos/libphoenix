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

#include <stdbool.h>
#include <assert.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/threads.h>
#include <sys/semaphore.h>
#include <time.h>


int semaphoreCount(semaphore_t *s)
{
	int ret = 0;

	if (s == NULL) {
		return -EINVAL;
	}

	ret = mutexTry(s->mutex);
	if (ret != EOK) {
		ret = 0;
	}
	else {
		ret = s->v;
		assert(ret >= 0); /* s->v is unsigned, we don't want to overflow */
		mutexUnlock(s->mutex);
	}

	return ret;
}


int semaphoreCreate(semaphore_t *s, unsigned int v)
{
	static const struct condAttr cAttr = { .clock = PH_CLOCK_MONOTONIC };
	int err;

	if (s == NULL || v > SEM_VALUE_MAX)
		return -EINVAL;

	err = mutexCreate(&s->mutex);
	if (err < 0) {
		return err;
	}

	err = condCreateWithAttr(&s->cond, &cAttr);
	if (err < 0) {
		resourceDestroy(s->mutex);
		return err;
	}

	s->v = v;

	return 0;
}


int semaphoreDown(semaphore_t *s, time_t timeout)
{
	time_t deadline = 0;

	if (timeout != 0) {
		time_t now;
		gettime(&now, NULL);
		deadline = now + timeout;
	}

	mutexLock(s->mutex);
	int err;
	do {
		if (s->v > 0) {
			--s->v;
			err = 0;
			break;
		}

		err = condWait(s->cond, s->mutex, deadline);
	} while (err != -ETIME);
	mutexUnlock(s->mutex);

	return err;
}


int semaphoreTryDown(semaphore_t *s)
{
	int ret = EOK;

	if (s != NULL) {
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
	}
	else {
		ret = -EINVAL;
	}

	mutexUnlock(s->mutex);
	return ret;
}


int semaphoreUp(semaphore_t *s)
{
	mutexLock(s->mutex);
	bool wasZero = (s->v == 0);
	++s->v;
	mutexUnlock(s->mutex);

	/* Phoenix specific - condSignal causes reschedule,
	 * so signal under mutex causes performance penalty.
	 * Conditionals are sticky, so there's no race risk.
	 */
	if (wasZero) {
		condSignal(s->cond);
	}

	return 0;
}


int semaphoreDone(semaphore_t *s)
{
	resourceDestroy(s->mutex);
	resourceDestroy(s->cond);

	return 0;
}
