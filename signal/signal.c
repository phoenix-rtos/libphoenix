/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Userspace posix signals
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski, Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <signal.h>
#include <sys/threads.h>
#include <phoenix/signal.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


extern int sys_tkill(int pid, int tid, int signal);


extern void _signal_trampoline(void);


int raise(int sig)
{
	return SET_ERRNO(sys_tkill(getpid(), gettid(), sig));
}


int kill(pid_t pid, int sig)
{
	return SET_ERRNO(sys_tkill(pid, 0, sig));
}


int killpg(pid_t pgrp, int sig)
{
	if (pgrp == 0) {
		pgrp = -getpgrp();
	}
	else {
		pgrp = -pgrp;
	}

	return SET_ERRNO(sys_tkill(pgrp, 0, sig));
}


void (*signal(int signum, void (*handler)(int)))(int)
{
	struct sigaction act;
	act.sa_handler = handler;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);

	struct sigaction oact = { 0 };
	if (sigaction(signum, &act, &oact) < 0) {
		return SIG_ERR;
	}
	return oact.sa_handler;
}


int sigaction(int sig, const struct sigaction *act, struct sigaction *oact)
{
	return SET_ERRNO(signalAction(sig, act, oact, _signal_trampoline));
}


int sigprocmask(int how, const sigset_t *set, sigset_t *oldset)
{
	unsigned int mask = 0u, mmask = 0u, old;

	if ((set == NULL) && (oldset == NULL)) {
		return EOK;
	}

	if (set != NULL) {
		if (how == SIG_BLOCK) {
			mask = 0xffffffffUL;
			mmask = *set;
		}
		else if (how == SIG_UNBLOCK) {
			mask = 0;
			mmask = *set;
		}
		else if (how == SIG_SETMASK) {
			mask = *set;
			mmask = 0xffffffffUL;
		}
		else {
			return SET_ERRNO(-EINVAL);
		}
	}

	old = signalMask(mask, mmask);
	if (oldset != NULL) {
		*oldset = old;
	}

	return EOK;
}


int sigsuspend(const sigset_t *sigmask)
{
	return SET_ERRNO(signalSuspend(*sigmask));
}


int sigfillset(sigset_t *set)
{
	if (set == NULL) {
		return SET_ERRNO(-EINVAL);
	}

	memset(set, ~0, sizeof(sigset_t));
	return 0;
}


int sigaddset(sigset_t *set, int signo)
{
	if ((set == NULL) || (signo >= NSIG) || (signo < 0)) {
		return SET_ERRNO(-EINVAL);
	}

	*set |= (1UL << signo);
	return 0;
}


int sigismember(const sigset_t *set, int signum)
{
	if ((set == NULL) || (signum >= NSIG) || (signum < 0)) {
		return SET_ERRNO(-EINVAL);
	}

	return !!(*set & (1UL << signum));
}


int sigemptyset(sigset_t *set)
{
	if (set == NULL) {
		return SET_ERRNO(-EINVAL);
	}

	memset(set, 0, sizeof(sigset_t));
	return 0;
}


int sigisemptyset(sigset_t *set)
{
	sigset_t empty;

	if (set == NULL) {
		return SET_ERRNO(-EINVAL);
	}

	(void)sigemptyset(&empty);

	return (memcmp(set, &empty, sizeof(sigset)) == 0);
}


int sigdelset(sigset_t *set, int signum)
{
	if ((set == NULL) || (signum >= NSIG) || (signum < 0)) {
		return SET_ERRNO(-EINVAL);
	}

	*set &= ~(1UL << signum);
	return 0;
}
