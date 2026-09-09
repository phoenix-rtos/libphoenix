/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * unistd pause
 *
 * Copyright 2023 Phoenix Systems
 * Author: Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <unistd.h>
#include <signal.h>
#include <pthread.h>


int pause(void)
{
	sigset_t mask;
	int ret;

	int oldval = _pthread_enable_asynccancel();
	(void)sigprocmask(SIG_BLOCK, NULL, &mask);
	ret = sigsuspend(&mask);
	_pthread_disable_asynccancel(oldval);

	return ret;
}
