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


#define set_errno(x) (x)


static sighandler_t _sightab[NSIG];


static const int _signals_phx2posix[] = { 0, SIGKILL, SIGSEGV, SIGILL, SIGFPE, SIGHUP, SIGINT, SIGQUIT, SIGTRAP,
	SIGABRT, SIGIOT, SIGEMT, SIGBUS, SIGSYS, SIGPIPE, SIGALRM, SIGTERM, SIGURG, SIGSTOP, SIGTSTP, SIGCONT, SIGCHLD,
	SIGTTIN, SIGTTOU, SIGIO, SIGXCPU, SIGXFSZ, SIGVTALRM, SIGPROF, SIGWINCH, SIGINFO, SIGUSR1, SIGUSR2 };


static const int _signals_posix2phx[] = { 0, 5, 6, 7, 3, 8, 9, 10, 11, 4, 1, 12, 2, 13, 14, 15, 16, 17, 18, 19,
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
	exit((sig & 0x7f) << 8);
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
		case SIGFPE: /* Should be handled by the kernel? */
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
		case SIGUSR1:
		case SIGUSR2:
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


static void _signal_handler(int phxsig)
{
	int sig;

	if (phxsig < 0 || phxsig >= NSIG) {
		/* Don't know what to do, terminate just in case */
		_signal_terminate(phxsig);
		/* Never reached */
	}

	/* Received Phoenix signal, need to convert it to POSIX signal */
	sig = _signals_phx2posix[phxsig];

	/* Invoke handler */
	(_sightab[sig])(sig);

	signalReturn(phxsig);
	/* Never reached */
}


int raise(int sig)
{
	return tkill(getpid(), gettid(), _signals_posix2phx[sig]);
}


int kill(pid_t pid, int sig)
{
	return set_errno(tkill(pid, 0, _signals_posix2phx[sig]));
}


void (*signal(int signum, void (*handler)(int)))(int)
{
	sighandler_t t;

	if (signum <= 0 || signum > NSIG) {
		(void)set_errno(EINVAL);
		return (sighandler_t)SIG_ERR;
	}

	if (!_signal_ismutable(signum)) {
		(void)set_errno(EINVAL);
		return (sighandler_t)SIG_ERR;
	}

	t = _sightab[signum];

	if (handler == SIG_DFL)
		_sightab[signum] = _signal_getdefault(signum);
	else if (handler == SIG_IGN)
		_sightab[signum] = _signal_ignore;
	else
		_sightab[signum] = handler;

	if (t == _signal_ignore)
		return SIG_IGN;
	else if (t == _signal_getdefault(signum))
		return SIG_DFL;
	else
		return t;
}


int sigaction(int sig, const struct sigaction * act, struct sigaction * oact)
{
	return -ENOSYS;
}


int sigprocmask(int how, const sigset_t *set, sigset_t *oldset)
{
	return -ENOSYS;
}


int sigsuspend(const sigset_t *sigmask)
{
	return -ENOSYS;
}


int sigfillset(sigset_t *set)
{
	return -ENOSYS;
}


int sigaddset(sigset_t *set, int signo)
{
	return -ENOSYS;
}


int sigismember(const sigset_t *set, int signum)
{
	return -ENOSYS;
}


int sigemptyset(sigset_t *set)
{
	return -ENOSYS;
}


int sigisemptyset(sigset_t *set)
{
	return -ENOSYS;
}


int sigdelset(sigset_t *set, int signum)
{
	return -ENOSYS;
}


void _signals_init(void)
{
	int i;

	/* Set default actions */
	for (i = 0; i < NSIG; ++i)
		_sightab[i] = _signal_getdefault(i);

	/* Register userspace handler */
	signalHandle(_signal_handler, 0xffffffffUL, 0xffffffffUL);
}
