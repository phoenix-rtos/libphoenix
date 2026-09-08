/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * resource.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski, Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _SYS_RESOURCE_H_
#define _SYS_RESOURCE_H_

#include <phoenix/limits.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>


#ifdef __cplusplus
extern "C" {
#endif


enum { RUSAGE_SELF,
	RUSAGE_CHILDREN = -1 };


struct rusage {
	struct timeval ru_utime;
	struct timeval ru_stime;
};


#define PRIO_PROCESS 0
#define PRIO_PGRP    1
#define PRIO_USER    2


extern int rlimits(limit_target_t target, int resource, struct rlimit *oldLimit, const struct rlimit *newLimit);


extern int getrusage(int who, struct rusage *usage);


static inline int getrlimit(int resource, struct rlimit *rlp)
{
	limit_target_t target = { .kind = limit_target_process, .pid = 0 };
	return SET_ERRNO(rlimits(target, resource, rlp, NULL));
}


static inline int setrlimit(int resource, const struct rlimit *rlp)
{
	limit_target_t target = { .kind = limit_target_process, .pid = 0 };
	return SET_ERRNO(rlimits(target, resource, NULL, rlp));
}


extern int getrlimit(int resource, struct rlimit *rlp);


extern int setrlimit(int resource, const struct rlimit *rlp);


extern int setpriority(int which, id_t who, int prio);


extern int getpriority(int which, id_t who);


#ifdef __cplusplus
}
#endif


#endif
