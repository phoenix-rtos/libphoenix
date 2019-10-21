/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * signal.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski, Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_SIGNAL_H_
#define _LIBPHOENIX_SIGNAL_H_

#include <time.h>
#include <phoenix/signal.h>


enum { SIG_BLOCK, SIG_SETMASK, SIG_UNBLOCK };


extern void (*bsd_signal(int, void (*)(int)))(int);


extern int kill(pid_t, int);


extern int killpg(pid_t, int);


extern int raise(int);


extern int sigaction(int, const struct sigaction *, struct sigaction *);


extern int sigaddset(sigset_t *, int);


extern int sigdelset(sigset_t *, int);


extern int sigemptyset(sigset_t *);


extern int sigfillset(sigset_t *);


extern int sighold(int);


extern int sigignore(int);


extern int siginterrupt(int, int);


extern int sigismember(const sigset_t *, int);


extern void (*signal(int, void (*)(int)))(int);


extern int sigpause(int);


extern int sigpending(sigset_t *);


extern int sigprocmask(int, const sigset_t *, sigset_t *);


extern int sigrelse(int);


extern void (*sigset(int, void (*)(int)))(int);


extern int sigsuspend(const sigset_t *);


extern int sigwait(const sigset_t *, int *);


#endif
