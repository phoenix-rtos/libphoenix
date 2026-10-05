/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * time: POSIX per-process timers
 *
 * Copyright 2026 Phoenix Systems
 * Author: Adam Greloch
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/threads.h>
#include <time.h>
#include <unistd.h>

#include "../common/util.h"
#include "../common/cpuclock.h"


int timerCreate(int clock, int pid, int tid, int notify, int signo);


int timerDelete(int id);


int timerSettime(int id, unsigned int flags, const timerspec_t *value, timerspec_t *old);


int timerGettime(int id, timerspec_t *value);


int timerGetoverrun(int id);


#define NOTIFY_STACKSZ 4096

/* How many callbacks one dispatcher pass may take; the rest wait for the next pass */
#define NOTIFY_BATCH 8


/*
 * SIGEV_THREAD timers only. The kernel still owns the timer and does the timing; this only
 * remembers which callback belongs to which handle and when the dispatcher should next look.
 */
typedef struct _timerNotify_t {
	struct _timerNotify_t *next;
	timer_t id;
	clockid_t clock;
	void (*function)(union sigval);
	union sigval value;
	time_t expiry;   /* absolute us on clock, 0 when disarmed */
	time_t interval; /* us, 0 for one-shot */
} timerNotify_t;


static struct {
	handle_t lock;
	handle_t cond;
	timerNotify_t *list;
	int threadStarted;
	pthread_once_t onceControl;
	char stack[NOTIFY_STACKSZ] __attribute__((aligned(8)));
} timer_common = {
	.onceControl = PTHREAD_ONCE_INIT
};


static int timer_clockToPhx(clockid_t clockid, int *pid, int *tid)
{
	int phxClock;

	*pid = 0;
	*tid = 0;

	if (CPUCLOCK_IS_DYNAMIC(clockid)) {
		if (CPUCLOCK_IS_THREAD(clockid)) {
			*tid = (int)CPUCLOCK_ID_VALUE(clockid);
			phxClock = PH_CLOCK_THREAD_CPUTIME;
		}
		else if (CPUCLOCK_IS_PROCESS(clockid)) {
			*pid = (int)CPUCLOCK_ID_VALUE(clockid);
			phxClock = PH_CLOCK_PROCESS_CPUTIME;
		}
		else {
			phxClock = -1;
		}

		return phxClock;
	}

	switch (clockid) {
		case CLOCK_MONOTONIC:
		case CLOCK_MONOTONIC_RAW:
			phxClock = PH_CLOCK_MONOTONIC;
			break;

		case CLOCK_REALTIME:
			phxClock = PH_CLOCK_REALTIME;
			break;

		case CLOCK_THREAD_CPUTIME_ID:
			phxClock = PH_CLOCK_THREAD_CPUTIME;
			break;

		case CLOCK_PROCESS_CPUTIME_ID:
			phxClock = PH_CLOCK_PROCESS_CPUTIME;
			break;

		default:
			phxClock = -1;
			break;
	}

	return phxClock;
}


static time_t timer_clockNow(clockid_t clockid)
{
	struct timespec now;

	if (clock_gettime(clockid, &now) < 0) {
		return 0;
	}

	return __timespecToUs(&now);
}


/*
 * SIGEV_THREAD dispatch
 */


static timerNotify_t *_timer_notifyFind(timer_t id)
{
	timerNotify_t *n = timer_common.list;

	while (n != NULL) {
		if (n->id == id) {
			break;
		}
		n = n->next;
	}

	return n;
}


/* Returns how long to wait for the next expiry, 0 meaning no timer is armed */
static time_t timer_notifyDispatch(void)
{
	struct {
		void (*function)(union sigval);
		union sigval value;
	} due[NOTIFY_BATCH];
	timerNotify_t *n;
	time_t now, sleep = 0, left;
	size_t i, ndue = 0;

	mutexLock(timer_common.lock);

	for (n = timer_common.list; (n != NULL) && (ndue < NOTIFY_BATCH); n = n->next) {
		if (n->expiry == 0) {
			continue;
		}

		now = timer_clockNow(n->clock);
		if (now < n->expiry) {
			continue;
		}

		due[ndue].function = n->function;
		due[ndue].value = n->value;
		ndue++;

		if (n->interval != 0) {
			/* Skip the periods that have gone by, the way the kernel timer does */
			n->expiry += ((now - n->expiry) / n->interval + 1) * n->interval;
		}
		else {
			n->expiry = 0;
		}
	}

	for (n = timer_common.list; n != NULL; n = n->next) {
		if (n->expiry == 0) {
			continue;
		}

		now = timer_clockNow(n->clock);
		left = (now >= n->expiry) ? 1 : (n->expiry - now);

		if ((sleep == 0) || (left < sleep)) {
			sleep = left;
		}
	}

	mutexUnlock(timer_common.lock);

	/* Run unlocked, as a callback may call back into timer_settime() */
	for (i = 0; i < ndue; i++) {
		due[i].function(due[i].value);
	}

	return (ndue != 0) ? 1 : sleep;
}


__attribute__((noreturn)) static void timer_notifyThread(void *arg)
{
	time_t sleep;

	(void)arg;

	for (;;) {
		sleep = timer_notifyDispatch();

		mutexLock(timer_common.lock);
		/* A zero timeout waits until timer_settime() signals that something was armed */
		(void)condWait(timer_common.cond, timer_common.lock, sleep);
		mutexUnlock(timer_common.lock);
	}
}


static void timer_notifyForkChild(void)
{
	timerNotify_t *n, *next;

	/*
	 * POSIX: the child inherits no timers, and the dispatcher thread did not come across the
	 * fork either. The lock and the condition stay usable, as Phoenix recreates a process'
	 * handles in its child, so they can serve the timers the child creates for itself.
	 */
	for (n = timer_common.list; n != NULL; n = next) {
		next = n->next;
		free(n);
	}

	timer_common.list = NULL;
	timer_common.threadStarted = 0;
}


static void timer_notifyInitOnce(void)
{
	if ((mutexCreate(&timer_common.lock) < 0) || (condCreate(&timer_common.cond) < 0)) {
		return;
	}

	(void)pthread_atfork(NULL, NULL, timer_notifyForkChild);
}


/* One dispatcher per process, started on the first SIGEV_THREAD timer it creates */
static int timer_notifyStart(void)
{
	sigset_t mask, orgMask;
	handle_t tid;
	int err = 0;

	if (timer_common.threadStarted != 0) {
		return 0;
	}

	/* Keep the dispatcher from being picked for signal delivery while it runs callbacks */
	sigfillset(&mask);
	pthread_sigmask(SIG_BLOCK, &mask, &orgMask);
	err = beginthreadex(timer_notifyThread, getPriority(), timer_common.stack,
			sizeof(timer_common.stack), NULL, &tid);
	pthread_sigmask(SIG_SETMASK, &orgMask, NULL);

	if (err < 0) {
		return err;
	}

	timer_common.threadStarted = 1;

	return 0;
}


static int timer_notifyRegister(timer_t id, clockid_t clockid, const struct sigevent *evp)
{
	timerNotify_t *n;
	int err;

	pthread_once(&timer_common.onceControl, timer_notifyInitOnce);

	n = calloc(1, sizeof(*n));
	if (n == NULL) {
		return -ENOMEM;
	}

	n->id = id;
	n->clock = clockid;
	n->function = evp->sigev_notify_function;
	n->value = evp->sigev_value;

	mutexLock(timer_common.lock);

	err = timer_notifyStart();
	if (err == 0) {
		n->next = timer_common.list;
		timer_common.list = n;
	}

	mutexUnlock(timer_common.lock);

	if (err != 0) {
		free(n);
	}

	return err;
}


static void timer_notifyUnregister(timer_t id)
{
	timerNotify_t *n, **prev;

	if (timer_common.threadStarted == 0) {
		return;
	}

	mutexLock(timer_common.lock);

	prev = &timer_common.list;
	for (n = timer_common.list; n != NULL; n = n->next) {
		if (n->id == id) {
			*prev = n->next;
			break;
		}
		prev = &n->next;
	}

	mutexUnlock(timer_common.lock);

	free(n);
}


/* value is as handed to timer_settime(), so it is read on the timer's own clock here too */
static void timer_notifyRearm(timer_t id, const timerspec_t *value, int absolute)
{
	timerNotify_t *n;

	if (timer_common.threadStarted == 0) {
		return;
	}

	mutexLock(timer_common.lock);

	n = _timer_notifyFind(id);
	if (n != NULL) {
		if (value->value == 0) {
			n->expiry = 0;
		}
		else {
			n->expiry = (absolute != 0) ? value->value : (timer_clockNow(n->clock) + value->value);
		}
		n->interval = value->interval;
	}

	mutexUnlock(timer_common.lock);

	if (n != NULL) {
		condSignal(timer_common.cond);
	}
}


/*
 * POSIX interface
 */


int timer_create(clockid_t clockid, struct sigevent *evp, timer_t *timerid)
{
	int phxClock, pid, tid, id, err;
	int notify = PH_TIMER_NOTIFY_SIGNAL, signo = SIGALRM, sigevThread = 0;

	if (timerid == NULL) {
		return SET_ERRNO(-EINVAL);
	}

	phxClock = timer_clockToPhx(clockid, &pid, &tid);
	if (phxClock < 0) {
		return SET_ERRNO(-EINVAL);
	}

	/* POSIX: a NULL sigevent means SIGALRM to the calling process */
	if (evp != NULL) {
		switch (evp->sigev_notify) {
			case SIGEV_NONE:
				notify = PH_TIMER_NOTIFY_NONE;
				signo = 0;
				break;

			case SIGEV_SIGNAL:
				notify = PH_TIMER_NOTIFY_SIGNAL;
				signo = evp->sigev_signo;
				break;

			case SIGEV_THREAD:
				if (evp->sigev_notify_function == NULL) {
					return SET_ERRNO(-EINVAL);
				}
				/* The kernel keeps the time, the dispatcher thread makes the call */
				notify = PH_TIMER_NOTIFY_NONE;
				signo = 0;
				sigevThread = 1;
				break;

			default:
				return SET_ERRNO(-EINVAL);
		}
	}

	id = timerCreate(phxClock, pid, tid, notify, signo);
	if (id < 0) {
		return SET_ERRNO(id);
	}

	if (sigevThread != 0) {
		err = timer_notifyRegister((timer_t)id, clockid, evp);
		if (err != 0) {
			(void)timerDelete(id);
			return SET_ERRNO(err);
		}
	}

	*timerid = (timer_t)id;

	return 0;
}


int timer_delete(timer_t timerid)
{
	int err = timerDelete((int)timerid);

	if (err == 0) {
		timer_notifyUnregister(timerid);
	}

	return SET_ERRNO(err);
}


int timer_settime(timer_t timerid, int flags, const struct itimerspec *value, struct itimerspec *ovalue)
{
	timerspec_t kvalue, kold;
	int err;

	if (value == NULL) {
		return SET_ERRNO(-EINVAL);
	}

	if (!__timespecValid(&value->it_value) || !__timespecValid(&value->it_interval) ||
			(value->it_value.tv_sec < 0) || (value->it_interval.tv_sec < 0)) {
		return SET_ERRNO(-EINVAL);
	}

	kvalue.value = __timespecToUs(&value->it_value);
	kvalue.interval = __timespecToUs(&value->it_interval);

	/*
	 * The kernel takes an exact zero it_value to mean "disarm", so a sub-microsecond one must
	 * not be allowed to round down into it.
	 */
	if ((kvalue.value == 0) && ((value->it_value.tv_sec != 0) || (value->it_value.tv_nsec != 0))) {
		kvalue.value = 1;
	}

	err = timerSettime((int)timerid, (unsigned int)flags, &kvalue, (ovalue != NULL) ? &kold : NULL);
	if (err < 0) {
		return SET_ERRNO(err);
	}

	if (ovalue != NULL) {
		__usToTimespec(kold.value, &ovalue->it_value);
		__usToTimespec(kold.interval, &ovalue->it_interval);
	}

	timer_notifyRearm(timerid, &kvalue, (((unsigned int)flags & TIMER_ABSTIME) != 0U) ? 1 : 0);

	return 0;
}


int timer_gettime(timer_t timerid, struct itimerspec *value)
{
	timerspec_t kvalue;
	int err;

	if (value == NULL) {
		return SET_ERRNO(-EINVAL);
	}

	err = timerGettime((int)timerid, &kvalue);
	if (err < 0) {
		return SET_ERRNO(err);
	}

	__usToTimespec(kvalue.value, &value->it_value);
	__usToTimespec(kvalue.interval, &value->it_interval);

	return 0;
}


int timer_getoverrun(timer_t timerid)
{
	return SET_ERRNO(timerGetoverrun((int)timerid));
}
