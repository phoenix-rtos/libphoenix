/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * unistd: alarm()
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski, Michał Mirosław
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/threads.h>
#include <sys/wait.h>
#include <sys/time.h>


static struct {
	char stack[128] __attribute__((aligned(8)));
	handle_t cond, lock;
	time_t wakeup;
	unsigned tid, dead;
} alarm_common;


__attribute__((noreturn))
static void alarm_thread(void *arg)
{
	time_t now;
	long long int sleep;

	mutexLock(alarm_common.lock);
	for (;;) {
		gettime(&now, NULL);
		sleep = alarm_common.wakeup - now;

		if (alarm_common.wakeup && sleep > 0)
			condWait(alarm_common.cond, alarm_common.lock, sleep);
		else
			break;
	}

	if (alarm_common.wakeup) {
		raise(SIGALRM);
		alarm_common.wakeup = 0;
	}

	alarm_common.dead = alarm_common.tid;
	alarm_common.tid = 0;
	mutexUnlock(alarm_common.lock);
	endthread();
}


unsigned int alarm(unsigned int seconds)
{
	time_t now, previous;

	if (!alarm_common.lock) {
		mutexCreate(&alarm_common.lock);
		condCreate(&alarm_common.cond);
	}

	mutexLock(alarm_common.lock);
	previous = alarm_common.wakeup;
	if (seconds) {
		gettime(&now, NULL);
		alarm_common.wakeup = now + 1000LL * 1000LL * seconds;
	}
	else {
		alarm_common.wakeup = 0;
	}

	if (alarm_common.tid) {
		condSignal(alarm_common.cond);
	}
	else {
		if (alarm_common.dead) {
			waittid(alarm_common.dead, 0);
			alarm_common.dead = 0;
		}

		beginthreadex(alarm_thread, priority(-1), alarm_common.stack, sizeof(alarm_common.stack), NULL, &alarm_common.tid);
	}
	mutexUnlock(alarm_common.lock);

	return previous ? (previous - now) / (1000 * 1000) : 0;
}
