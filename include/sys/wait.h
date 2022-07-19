/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/wait.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski, Krystian Wasik
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _SYS_WAIT_H_
#define _SYS_WAIT_H_

#include <sys/types.h>


#ifdef __cplusplus
extern "C" {
#endif


enum {
	WNOHANG = 1 << 0,
	WUNTRACED = 1 << 1,
};


extern const int _signals_phx2posix[];


#define WTERMSIG(stat_val) (_signals_phx2posix[(stat_val >> 8) & 0x7f])
#define WEXITSTATUS(stat_val) ((stat_val) & 0xff)
#define WIFEXITED(stat_val) (WTERMSIG(stat_val) == 0)
#define WIFSIGNALED(stat_val) (WTERMSIG(stat_val) != 0)
#define WIFSTOPPED(stat_val) 0
#define WSTOPSIG(stat_val) 0
#define WIFCONTINUED(stat_val) 0
#define WCOREDUMP(stat_val) 0


extern pid_t waitpid(pid_t pid, int *status, int options);


static inline pid_t wait(int *status)
{
	return waitpid(-1, status, 0);
}


#ifdef __cplusplus
}
#endif


#endif
