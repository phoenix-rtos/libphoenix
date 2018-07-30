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


#include <sys/types.h>
#include <time.h>


#define SIGHUP     1
#define SIGINT     2
#define SIGQUIT    3
#define SIGILL     4
#define SIGTRAP    5
#define SIGABRT    6
#define SIGIOT     SIGABRT
#define SIGEMT     7
#define SIGFPE     8
#define SIGKILL    9
#define SIGBUS    10
#define SIGSEGV   11
#define SIGSYS    12
#define SIGPIPE   13
#define SIGALRM   14
#define SIGTERM   15
#define SIGURG    16
#define SIGSTOP   17
#define SIGTSTP   18
#define SIGCONT   19
#define SIGCHLD   20
#define SIGTTIN   21
#define SIGTTOU   22
#define SIGIO     23
#define SIGXCPU   24
#define SIGXFSZ   25
#define SIGVTALRM 26
#define SIGPROF   27
#define SIGWINCH  28
#define SIGINFO   29
#define SIGUSR1   30
#define SIGUSR2   31

#define NSIG 32

#define SIG_ERR (-1)
#define SIG_DFL (-2)
#define SIG_IGN (-3)


enum { SIG_BLOCK, SIG_SETMASK, SIG_UNBLOCK };


enum {
	SA_NOCLDSTOP = 1 << 0,
	SA_NOCLDWAIT = 1 << 1,
	SA_NODEFER = 1 << 2,
	SA_ONSTACK = 1 << 3,
	SA_RESETHAND = 1 << 4,
	SA_RESTART = 1 << 5,
	SA_RESTORER = 1 << 6,
	SA_SIGINFO = 1 << 7,
};


typedef void (*sighandler_t)(int);


typedef int sigset_t;
typedef int sig_atomic_t;


union sigval {
	int   sival_int;
	void *sival_ptr;
};


typedef struct {
	int     si_signo;
	int     si_code;
	int     si_errno;
	pid_t   si_pid;
	uid_t   si_uid;
	void   *si_addr;
	int     si_status;
	long    si_band;
	union sigval si_value;
} siginfo_t;


struct sigaction {
	void (*sa_handler) (int);
	sigset_t sa_mask;
	int sa_flags;
	void (*sa_sigaction) (int, siginfo_t *, void *);
};


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


extern int sigqueue(pid_t, int, const union sigval);


extern int sigrelse(int);


extern void (*sigset(int, void (*)(int)))(int);


extern int sigsuspend(const sigset_t *);


extern int sigtimedwait(const sigset_t *, siginfo_t *, const struct timespec *);


extern int sigwait(const sigset_t *, int *);


extern int sigwaitinfo(const sigset_t *, siginfo_t *);


#endif
