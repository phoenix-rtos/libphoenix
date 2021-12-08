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
#include <phoenix/posix/signal.h>
#include <phoenix/posix/timespec.h>


/* Signal flags */
#define SA_NOCLDSTOP (1 << 0) /* Do not generate SIGCHLD when children stop or stopped children continue */
#define SA_NOCLDWAIT (1 << 1) /* Causes implementations not to create zombie processes or status information on child termination */
#define SA_NODEFER   (1 << 2) /* Causes signal not to be automatically blocked on entry to signal handler */
#define SA_ONSTACK   (1 << 3) /* Process is executing on an alternate signal stack */
#define SA_RESETHAND (1 << 4) /* Causes signal dispositions to be set to SIG_DFL on entry to signal handlers */
#define SA_RESTART   (1 << 5) /* Causes certain functions to become restartable */
#define SA_RESTORER  (1 << 6) /* Indicates that the sa_restorer field containsthe address of a 'signal trampoline' */
#define SA_SIGINFO   (1 << 7) /* Causes extra information to be passed to signal handlers at the time of receipt of a signal */


/* Signal action */
enum {
	SIG_BLOCK,   /* For blocking signals */
	SIG_UNBLOCK, /* For unblocking signals */
	SIG_SETMASK  /* For setting signal mask */
};


typedef int sig_atomic_t; /* Used for atomic access, even in the presence of asynchronous interrupts */
typedef int sigset_t;     /* Used to represent sets of signals */



union sigval {
	int sival_int;   /* Integer signal value */
	void *sival_ptr; /* Pointer signal value */
};


typedef struct {
	int si_signo;          /* Signal number */
	int si_code;           /* Signal code */
	int si_errno;          /* If non-zero, an errno value associated with this signal */
	pid_t si_pid;          /* Sending process ID */
	uid_t si_uid;          /* Real user ID of sending process */
	void *si_addr;         /* Address of faulting instruction */
	int si_status;         /* Exit value or signal */
	long si_band;          /* Band event for SIGPOLL */
	union sigval si_value; /* Signal value */
} siginfo_t;


struct sigaction {
	void (*sa_handler) (int);                        /* Pointer to a signal-catching function or SIG_IGN/SIG_DFL */
	sigset_t sa_mask;                                /* Set of signals to be blocked during execution of the signal handling function */
	int sa_flags;                                    /* Special flags */
	void (*sa_sigaction) (int, siginfo_t *, void *); /* Pointer to a signal-catching function */
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


extern int sigrelse(int);


extern void (*sigset(int, void (*)(int)))(int);


extern int sigsuspend(const sigset_t *);


extern int sigwait(const sigset_t *, int *);


#endif
