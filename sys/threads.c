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


typedef struct {
	void *arg;
	void (*start)(void *);
} wrapperArgs_t;


static void wrapper(void *arg)
{
	wrapperArgs_t *ctx = (wrapperArgs_t *)arg;

	ctx->start(ctx->arg);

	free(ctx);
	endthread();
}


int beginthread(void (*start)(void *), unsigned int priority, void *stack, unsigned int stacksz, void *arg)
{
	wrapperArgs_t *ctx;
	int ret;

	ctx = malloc(sizeof(wrapperArgs_t));
	if (ctx == NULL) {
		return -ENOMEM;
	}
	ctx->start = start;
	ctx->arg = arg;

	ret = beginthreadex(wrapper, priority, stack, stacksz, ctx, NULL);
	if (ret < 0) {
		free(ctx);
	}

	return ret;
}
