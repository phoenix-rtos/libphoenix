/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * unistd: alarm()
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski, Michal Miroslaw
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
	char stack[1024] __attribute__((aligned(8)));
	handle_t cond, lock;
	time_t wakeup;
	unsigned tid;
} alarm_common;


__attribute__((noreturn))
static void alarm_thread(void *arg)
{
	time_t now;
	long long int sleep;

	signalMask(0xffffffff, 0xffffffff);
	mutexLock(alarm_common.lock);

	for (;;) {
		gettime(&now, NULL);
		sleep = alarm_common.wakeup - now;

		if (!alarm_common.wakeup) {
			condWait(alarm_common.cond, alarm_common.lock, 0);
		}
		else if (sleep > 0) {
			condWait(alarm_common.cond, alarm_common.lock, sleep);
		}
		else {
			kill(getpid(), SIGALRM);
			alarm_common.wakeup = 0;
		}
	}
}


unsigned int alarm(unsigned int seconds)
{
	time_t now, previous;

	if (!alarm_common.tid) {
		mutexCreate(&alarm_common.lock);
		condCreate(&alarm_common.cond);
		beginthreadex(alarm_thread, priority(-1), alarm_common.stack, sizeof(alarm_common.stack), NULL, &alarm_common.tid);
	}

	mutexLock(alarm_common.lock);
	previous = alarm_common.wakeup;
	gettime(&now, NULL);
	if (seconds)
		alarm_common.wakeup = now + 1000LL * 1000LL * seconds;
	else
		alarm_common.wakeup = 0;
	mutexUnlock(alarm_common.lock);
	condSignal(alarm_common.cond);

	return previous ? (previous - now) / (1000 * 1000) : 0;
}
