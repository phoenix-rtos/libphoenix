/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/threads
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <sys/threads.h>
#include <errno.h>


int mutexCreate(handle_t *h)
{
	static const struct lockAttr defaultAttr = { .type = PH_LOCK_NORMAL };

	return phMutexCreate(h, &defaultAttr);
}


int mutexLock(handle_t m)
{
	int err;
	while ((err = phMutexLock(m, 0, -1)) == -EINTR)
		;
	return err;
}


int mutexLockTimeoutable(handle_t m, time_t timeout, int clock)
{
	int err;
	while ((err = phMutexLock(m, timeout, clock)) == -EINTR)
		;
	return err;
}


int condCreate(handle_t *h)
{
	static const struct condAttr defaultAttr = { .clock = PH_CLOCK_RELATIVE };

	return phCondCreate(h, &defaultAttr);
}


int condClockWait(handle_t h, handle_t m, time_t timeout, int clock)
{
	int err, mut_err;

	err = phCondWait(h, m, timeout, clock);

	while (err == -EINTR) {
		mut_err = mutexLock(m);
		if (mut_err != EOK) {
			return mut_err;
		}
		err = phCondWait(h, m, timeout, clock);
	}

	return err;
}


int condWait(handle_t h, handle_t m, time_t timeout)
{
	return condClockWait(h, m, timeout, -1);
}


int mutexWait(handle_t m, time_t timeout, int clock)
{
	int err;

	do {
		err = phMutexWait(m, timeout, clock);
	} while (err == -EINTR);

	return err;
}


int mutexLock2(handle_t m1, handle_t m2)
{
	int err;
	int tmp;

	if ((err = mutexLock(m1)) < 0)
		return err;

	while (mutexTry(m2) < 0) {
		mutexUnlock(m1);
		if ((err = mutexLock(m2)) < 0)
			return err;

		tmp = m1;
		m1 = m2;
		m2 = tmp;
	}

	return EOK;
}
