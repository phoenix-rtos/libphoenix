/*
 * Phoenix-RTOS
 *
 * Semaphores
 *
 * Copyright 2012, 2017, 2018, 2026 Phoenix Systems
 * Copyright 2006 Pawel Pisarczyk
 * Author: Pawel Pisarczyk, Aleksander Kaminski, Michal Lach, Ziemowit Leszczynski
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdbool.h>
#include <limits.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/threads.h>
#include <time.h>


int semaphoreCount(semaphore_t *s)
{
	int ret;

	if (s == NULL) {
		return -EINVAL;
	}

	mutexLock(s->mutex);
	ret = (int)s->v;
	mutexUnlock(s->mutex);

	return ret;
}


int semaphoreCreate(semaphore_t *s, unsigned int v)
{
	static const struct condAttr cAttr = { .clock = PH_CLOCK_MONOTONIC, .type = PH_COND_NORMAL };
	int err;

	if (s == NULL || v > SEM_VALUE_MAX) {
		return -EINVAL;
	}

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


int semaphoreDownAtClock(semaphore_t *s, time_t deadline, int clock)
{
	int err;

	if (s == NULL) {
		return -EINVAL;
	}

	if ((clock != PH_CLOCK_REALTIME) && (clock != PH_CLOCK_MONOTONIC)) {
		return -EINVAL;
	}

	mutexLock(s->mutex);

	do {
		if (s->v > 0) {
			--s->v;
			err = 0;
			break;
		}

		/*
		 * An absolute deadline at or before the epoch has always passed. It has
		 * to be answered here: condClockWait() reads 0 as "no deadline" and
		 * rejects a negative one with -EINVAL.
		 */
		if (deadline <= 0) {
			err = -ETIME;
			break;
		}

		err = condClockWait(s->cond, s->mutex, deadline, clock);
	} while ((err == 0) || (err == -EINTR));

	mutexUnlock(s->mutex);

	return err;
}


int semaphoreTryDown(semaphore_t *s)
{
	int ret;

	if (s == NULL) {
		return -EINVAL;
	}

	mutexLock(s->mutex);
	if (s->v > 0) {
		--s->v;
		ret = EOK;
	}
	else {
		ret = -EAGAIN;
	}
	mutexUnlock(s->mutex);

	return ret;
}


int semaphoreUp(semaphore_t *s)
{
	bool wasZero = false;
	int ret = 0;

	mutexLock(s->mutex);
	if (s->v >= SEM_VALUE_MAX) {
		ret = -EOVERFLOW;
	}
	else {
		wasZero = (s->v == 0);
		++s->v;
	}
	mutexUnlock(s->mutex);

	/* Phoenix specific - condSignal causes reschedule,
	 * so signal under mutex causes performance penalty.
	 * Conditionals are sticky, so there's no race risk.
	 */
	if (wasZero) {
		condSignal(s->cond);
	}

	return ret;
}


int semaphoreDone(semaphore_t *s)
{
	resourceDestroy(s->mutex);
	resourceDestroy(s->cond);

	return 0;
}
