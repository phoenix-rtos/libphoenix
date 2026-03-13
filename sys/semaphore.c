/*
 * Phoenix-RTOS
 *
 * Semaphores
 *
 * Copyright 2012, 2017, 2018, 2026 Phoenix Systems
 * Copyright 2006 Pawel Pisarczyk
 * Author: Pawel Pisarczyk, Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <stdbool.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/threads.h>
#include <time.h>


int semaphoreCreate(semaphore_t *s, unsigned int v)
{
	static const struct condAttr cAttr = { .clock = PH_CLOCK_MONOTONIC };
	int err;

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
