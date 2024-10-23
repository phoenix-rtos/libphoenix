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
#include <stdlib.h>


int __isthreaded;


int mutexCreate(handle_t *h)
{
	static const struct lockAttr defaultAttr = { .type = PH_LOCK_NORMAL };

	return phMutexCreate(h, &defaultAttr);
}


int mutexLock(handle_t m)
{
	int err;
	while ((err = phMutexLock(m)) == -EINTR) ;
	return err;
}


int condCreate(handle_t *h)
{
	static const struct condAttr defaultAttr = { .clock = PH_CLOCK_RELATIVE };

	return phCondCreate(h, &defaultAttr);
}


int condWait(handle_t h, handle_t m, time_t timeout)
{
	int err, mut_err;

	err = phCondWait(h, m, timeout);

	while (err == -EINTR) {
		mut_err = mutexLock(m);
		if (mut_err != EOK) {
			return mut_err;
		}
		err = phCondWait(h, m, timeout);
	}

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


int beginthreadex(void (*start)(void *), unsigned int priority, void *stack, unsigned int stacksz, void *arg, handle_t *id)
{
	struct tls_tcb *tcb;
	int ret;

	ret = __tls_alloc(&tcb);
	if (ret != 0) {
		return ret;
	}

	/* Inform that we are threaded. */
	if (__isthreaded == 0) {
		/* Make sure errno in initthread is set correctly. */
		int olderrno = errno;
		__isthreaded = 1;
		errno = olderrno;
	}

	return beginthreadexsvc(start, priority, stack, stacksz, arg, id, __tls_tcbPtrToTlsPtr(tcb));
}
