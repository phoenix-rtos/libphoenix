/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * posix signals
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <signal.h>
#include <errno.h>


static void sig_default(int v)
{
}


void (*SIG_DFL)(int) = sig_default;
void (*SIG_IGN)(int) = sig_default;


int raise(int sig)
{
	return -ENOSYS;
}


void (*signal(int signum, void (*handler)(int)))(int)
{
	return NULL;
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
