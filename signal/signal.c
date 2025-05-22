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


struct {
	sighandler_t sightab[NSIG];
	sigset_t sigset[NSIG];
	handle_t lock;
} signal_common;


const int _signals_phx2posix[] = { 0, SIGKILL, SIGSEGV, SIGILL, SIGFPE, SIGHUP, SIGINT, SIGQUIT, SIGTRAP,
	SIGABRT, SIGEMT, SIGBUS, SIGSYS, SIGPIPE, SIGALRM, SIGTERM, SIGURG, SIGSTOP, SIGTSTP, SIGCONT, SIGCHLD,
	SIGTTIN, SIGTTOU, SIGIO, SIGXCPU, SIGXFSZ, SIGVTALRM, SIGPROF, SIGWINCH, SIGINFO, SIGUSR1, SIGUSR2 };


static const int _signals_posix2phx[] = { 0, 5, 6, 7, 3, 8, 9, 10, 4, 1, 11, 2, 12, 13, 14, 15, 16, 17, 18, 19,
	20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 };


static void _signal_bug(int sig)
{
#ifndef NDEBUG
	printf("%u: BUG - received signal #0\n", getpid());
#endif
}


static void _signal_ignore(int sig)
{
}


static void _signal_terminate(int sig)
{
	int phxsig = _signals_posix2phx[sig];

	_exit(((phxsig) & 0x7f) << 8);
}


static void (*_signal_getdefault(int sig))(int)
{
	switch (sig) {
		case 0:
			return _signal_bug;

		case SIGHUP:
		case SIGINT:
		case SIGQUIT:
		case SIGILL:
		case SIGTRAP:
		case SIGABRT: /* And SIGIOT */
		case SIGEMT:
		case SIGFPE:  /* Should be handled by the kernel? */
		case SIGKILL: /* Should kill the process before handler is ever invoked */
		case SIGBUS:
		case SIGSEGV:
		case SIGSYS:
		case SIGPIPE:
		case SIGALRM:
		case SIGTERM:
		case SIGIO:
		case SIGXCPU:
		case SIGXFSZ:
		case SIGVTALRM:
		case SIGPROF:
		case SIGUSR1:
		case SIGUSR2:
			return _signal_terminate;

		case SIGURG:
		case SIGSTOP: /* TODO: Stop process. Should be handled by the kernel? */
		case SIGTSTP: /* TODO: Stop process. Should be handled by the kernel? */
		case SIGCONT: /* TODO: Continue process. Should be handled by the kernel? */
		case SIGCHLD:
		case SIGTTIN: /* TODO: Stop process. Should be handled by the kernel? */
		case SIGTTOU: /* TODO: Stop process. Should be handled by the kernel? */
		case SIGWINCH:
		case SIGINFO:
			return _signal_ignore;

		default:
			return NULL;
	}
}


static int _signal_ismutable(int sig)
{
	switch (sig) {
		case SIGKILL:
		case SIGSTOP:
			return 0;

		default:
			return 1;
	}
}


void _signal_handler(int phxsig)
{
	int sig;

	if ((phxsig < 0) || (phxsig >= NSIG)) {
		/* Don't know what to do, ignore it */
		return;
	}

	/* Received Phoenix signal, need to convert it to POSIX signal */
	sig = _signals_phx2posix[phxsig];

	/* POSIX: sa_mask should be ORed with current process signal mask */
	signalMask(signal_common.sigset[sig], signal_common.sigset[sig]);

	/* Invoke handler */
	(signal_common.sightab[sig])(sig);

	/* oldmask kept on stack and restored by sigreturn */
}


int raise(int sig)
{
	return SET_ERRNO(sys_tkill(getpid(), gettid(), _signals_posix2phx[sig]));
}


int kill(pid_t pid, int sig)
{
	return SET_ERRNO(sys_tkill(pid, 0, _signals_posix2phx[sig]));
}


int killpg(pid_t pgrp, int sig)
{
	if (pgrp == 0) {
		pgrp = -getpgrp();
	}
	else {
		pgrp = -pgrp;
	}

	return SET_ERRNO(sys_tkill(pgrp, 0, _signals_posix2phx[sig]));
}


void (*signal(int signum, void (*handler)(int)))(int)
{
	sighandler_t t;
	unsigned int oldmask;

	if ((signum <= 0) || (signum >= NSIG)) {
		(void)SET_ERRNO(-EINVAL);
		return SIG_ERR;
	}

	if (_signal_ismutable(signum) == 0) {
		(void)SET_ERRNO(-EINVAL);
		return SIG_ERR;
	}

	/* Mask signal before change */
	mutexLock(signal_common.lock);
	oldmask = signalMask(1UL << _signals_posix2phx[signum], 0xffffffffUL);

	t = signal_common.sightab[signum];

	if (handler == SIG_DFL) {
		signal_common.sightab[signum] = _signal_getdefault(signum);
	}
	else if (handler == SIG_IGN) {
		signal_common.sightab[signum] = _signal_ignore;
	}
	else {
		signal_common.sightab[signum] = handler;
	}

	signalMask(oldmask, 0xffffffffUL);
	mutexUnlock(signal_common.lock);

	if (t == _signal_ignore) {
		return SIG_IGN;
	}
	else if (t == _signal_getdefault(signum)) {
		return SIG_DFL;
	}
	else {
		return t;
	}
}


/* TODO: Handle flags */
int sigaction(int sig, const struct sigaction *act, struct sigaction *oact)
{
	unsigned int oldmask;
	int i;

	if ((sig <= 0) || (sig >= NSIG)) {
		return SET_ERRNO(-EINVAL);
	}

	if (oact != NULL) {
		mutexLock(signal_common.lock);
		if (signal_common.sightab[sig] == _signal_ignore) {
			oact->sa_handler = (sighandler_t)SIG_IGN;
		}
		else if (signal_common.sightab[sig] == _signal_getdefault(sig)) {
			oact->sa_handler = (sighandler_t)SIG_DFL;
		}
		else {
			oact->sa_handler = signal_common.sightab[sig];
		}

		/* convert phx signals to POSIX */
		oact->sa_mask = 0;
		for (i = 0; i < NSIG; ++i) {
			if (signal_common.sigset[sig] & (1UL << i)) {
				/* FIXME: support SA_NODEFER correctly (do not modify sa_mask when installing handler) */
				oact->sa_mask |= 1UL << _signals_phx2posix[i];
			}
		}

		oact->sa_flags = 0; /* TODO: flags */
		mutexUnlock(signal_common.lock);
	}

	if (act != NULL) {
		if (_signal_ismutable(sig) == 0) {
			return SET_ERRNO(-EINVAL);
		}

		/* Mask signal before change */
		mutexLock(signal_common.lock);
		oldmask = signalMask(1UL << _signals_posix2phx[sig], 0xffffffffUL);

		if (act->sa_handler == (sighandler_t)SIG_IGN) {
			signal_common.sightab[sig] = _signal_ignore;
		}
		else if (act->sa_handler == (sighandler_t)SIG_DFL) {
			signal_common.sightab[sig] = _signal_getdefault(sig);
		}
		else {
			signal_common.sightab[sig] = act->sa_handler;
		}

		for (i = 0, signal_common.sigset[sig] = 0; i < NSIG; ++i) {
			if (act->sa_mask & (1UL << i)) {
				signal_common.sigset[sig] |= (1UL << _signals_posix2phx[i]);
			}
		}

		if ((act->sa_flags & SA_NODEFER) == 0) {
			signal_common.sigset[sig] |= 1UL << _signals_posix2phx[sig];
		}

		signalMask(oldmask, 0xffffffffUL);
		mutexUnlock(signal_common.lock);
	}

	if ((oact == NULL) && (act == NULL)) {
		return SET_ERRNO(-EINVAL);
	}

	return EOK;
}


int sigprocmask(int how, const sigset_t *set, sigset_t *oldset)
{
	int i;
	unsigned int phxv = 0u, mask = 0u, old;

	if ((set == NULL) && (oldset == NULL)) {
		return EOK;
	}

	if (set != NULL) {
		for (i = 0; i < NSIG; ++i) {
			if ((*set & (1UL << i)) && (_signal_ismutable(i) != 0)) {
				phxv |= (1UL << _signals_posix2phx[i]);
			}
		}

		if (how == SIG_BLOCK) {
			mask = phxv;
			phxv = 0xffffffffUL;
		}
		else if (how == SIG_UNBLOCK) {
			mask = phxv;
			phxv = 0;
		}
		else if (how == SIG_SETMASK) {
			mask = 0xffffffffUL;
		}
		else {
			return SET_ERRNO(-EINVAL);
		}
	}

	old = signalMask(phxv, mask);

	if (oldset != NULL) {
		memset(oldset, 0, sizeof(sigset_t));
		for (i = 0; i < NSIG; ++i) {
			if (old & (1UL << i)) {
				(*oldset) |= (1UL << _signals_phx2posix[i]);
			}
		}
	}

	return EOK;
}


int sigsuspend(const sigset_t *sigmask)
{
	unsigned int phxv = 0u, i;

	for (i = 0; i < NSIG; ++i) {
		if (*sigmask & (1UL << i)) {
			phxv |= (1UL << _signals_posix2phx[i]);
		}
	}

	return SET_ERRNO(signalSuspend(phxv));
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


int signalPostPosix(int pid, int tid, int signal)
{
	return signalPost(pid, tid, _signals_posix2phx[signal]);
}


extern void _signal_trampoline(void);


void _signals_init(void)
{
	int i;

	/* Set default actions */
	for (i = 0; i < NSIG; ++i) {
		signal_common.sightab[i] = _signal_getdefault(i);
		signal_common.sigset[i] = 1UL << _signals_posix2phx[i];
	}

	mutexCreate(&signal_common.lock);

	/* Register userspace handler */
	signalHandle(_signal_trampoline, 0, 0xffffffffUL);
}
