/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * abort.c
 *
 * Copyright 2018,2023 Phoenix Systems
 * Author: Kamil Amanowicz, Marek Bialowas
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void abort(void)
{
	/* unblock SIGABRT */
	sigset_t sigs;
	sigemptyset(&sigs);
	sigaddset(&sigs, SIGABRT);
	sigprocmask(SIG_UNBLOCK, &sigs, NULL);

	raise(SIGABRT);

	/* should not return even for user-specified signal handler, but retry with default handler */
	signal(SIGABRT, SIG_DFL);
	raise(SIGABRT);

	/* should not return, finish program however we can */
	for (;;) {
		_exit(127);
	}
}
