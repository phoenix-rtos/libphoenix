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


#include <sys/debug.h>
#include <stdlib.h>

/* Provide stub definition of tls managing functions for statically linked programs. */

static void _libphoenix_stub(void)
{
}

extern void __attribute__((weak, alias("_libphoenix_stub"))) _rtld_tls_free_curr(void);
extern void __attribute__((weak, alias("_libphoenix_stub"))) _rtld_tls_allocate_curr(void);


/* FIXME: Hack to provide tls support in dynamically loaded binaries. */
void beginthreadex_wrapper(void *arg)
{
	_rtld_tls_allocate_curr();

	void *(*start)(void *) = ((void **)arg)[0];
	void *startArg = ((void **)arg)[1];

	free(arg);

	start(startArg);
}

int beginthreadex(void (*start)(void *), unsigned int priority, void *stack, unsigned int stacksz, void *arg, handle_t *id)
{
	void **startAndArg = (void **)malloc(sizeof(void *) * 2);
	startAndArg[0] = start;
	startAndArg[1] = arg;
	return beginthreadexsvc(beginthreadex_wrapper, priority, stack, stacksz, startAndArg, id);
}


__attribute__((noreturn)) void endthread(void)
{
	_rtld_tls_free_curr();
	endthreadsvc();
}


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
