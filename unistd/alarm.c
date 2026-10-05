/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * unistd: alarm()
 *
 * Copyright 2018, 2026 Phoenix Systems
 * Author: Jan Sikorski, Michal Miroslaw, Adam Greloch
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <limits.h>
#include <unistd.h>


/*
 * One kernel timer serves every alarm() of the process. The kernel keeps the deadline and
 * raises SIGALRM itself, so nothing here has to wait on it.
 */
static struct {
	timer_t id;
	int created;
	pthread_once_t onceControl;
} alarm_common = {
	.onceControl = PTHREAD_ONCE_INIT
};


static void alarm_create(void)
{
	struct sigevent evp;

	evp.sigev_notify = SIGEV_SIGNAL;
	evp.sigev_signo = SIGALRM;
	evp.sigev_value.sival_int = 0;
	evp.sigev_notify_function = NULL;
	evp.sigev_notify_attributes = NULL;

	alarm_common.created = (timer_create(CLOCK_MONOTONIC, &evp, &alarm_common.id) == 0) ? 1 : 0;
}


static void alarm_forkChild(void)
{
	/* POSIX: alarms are cleared in the child, which inherits no timers either */
	alarm_create();
}


static void alarm_initOnce(void)
{
	alarm_create();
	(void)pthread_atfork(NULL, NULL, alarm_forkChild);
}


unsigned int alarm(unsigned int seconds)
{
	struct itimerspec value, old;
	unsigned int remaining;

	pthread_once(&alarm_common.onceControl, alarm_initOnce);

	if (alarm_common.created == 0) {
		return 0;
	}

	value.it_interval.tv_sec = 0;
	value.it_interval.tv_nsec = 0;
	value.it_value.tv_sec = (time_t)seconds;
	value.it_value.tv_nsec = 0;

	if (timer_settime(alarm_common.id, 0, &value, &old) < 0) {
		return 0;
	}

	/* POSIX: a part of a second left over is reported as a whole second */
	remaining = (unsigned int)old.it_value.tv_sec;
	if ((old.it_value.tv_nsec != 0) && (remaining < UINT_MAX)) {
		remaining++;
	}

	return remaining;
}
