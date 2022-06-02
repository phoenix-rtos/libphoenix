/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * pthread
 *
 * Copyright 2017-2018, 2019 Phoenix Systems
 * Author: Pawel Pisarczyk, Michał Mirosław, Marcin Baran
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_PTHREAD_H_
#define _LIBPHOENIX_PTHREAD_H_

#include <sys/types.h>
#include <sys/threads.h>
#include <sched.h>


#define EDEADLK 38

#define PTHREAD_CREATE_JOINABLE 0
#define PTHREAD_CREATE_DETACHED 1

#define PTHREAD_EXPLICIT_SCHED 0
#define PTHREAD_INHERIT_SCHED 1

#define PTHREAD_PRIO_NONE 0
#define PTHREAD_PRIO_INHERIT 1
#define PTHREAD_PRIO_PROTECT 2

#define PTHREAD_PROCESS_SHARED 0
#define PTHREAD_PROCESS_PRIVATE 1

#define PTHREAD_SCOPE_PROCESS 0
#define PTHREAD_SCOPE_SYSTEM 1

#define PTHREAD_MUTEX_INITIALIZER 0

#define PTHREAD_MUTEX_DEFAULT 0
#define PTHREAD_MUTEX_ERRORCHECK 1
#define PTHREAD_MUTEX_NORMAL 2
#define PTHREAD_MUTEX_RECURSIVE 3


extern int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
	void *(*start_routine)(void *), void *arg);


extern int pthread_join(pthread_t thread, void **value_ptr);


extern int pthread_detach(pthread_t thread);


extern pthread_t pthread_self(void);


extern int pthread_equal(pthread_t t1, pthread_t t2);


extern void pthread_exit(void *value_ptr);


extern int pthread_attr_init(pthread_attr_t *attr);


extern int pthread_attr_destroy(pthread_attr_t *attr);


extern int pthread_attr_setstackaddr(pthread_attr_t *attr, void *stackaddr);


extern int pthread_attr_getstackaddr(const pthread_attr_t *attr,
	void **stackaddr);


extern int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize);


extern int pthread_attr_getstacksize(const pthread_attr_t *attr,
	size_t *stacksize);


extern int pthread_attr_setschedparam(pthread_attr_t *attr,
	const struct sched_param *param);


extern int pthread_attr_getschedparam(const pthread_attr_t *attr,
	struct sched_param *param);


extern int pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy);


extern int pthread_attr_getschedpolicy(const pthread_attr_t *attr,
	int *policy);


extern int pthread_attr_setscope(pthread_attr_t *attr, int contentionscope); //TODO


extern int pthread_attr_getscope(const pthread_attr_t *attr,
	int *contentionscope); //TODO


extern int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate);


extern int pthread_attr_getdetachstate(const pthread_attr_t *attr,
	int *detachstate);


extern int pthread_attr_getinheritsched(const pthread_attr_t *restrict attr,
	int *restrict inheritsched); //TODO


extern int pthread_attr_setinheritsched(pthread_attr_t *attr,
	int inheritsched); //TODO


extern int pthread_setschedprio(pthread_t thread, int prio);


extern int pthread_getschedparam(pthread_t thread, int *policy,
	struct sched_param *restrict param);


extern int pthread_setschedparam(pthread_t thread, int policy,
	const struct sched_param *param);


extern int pthread_mutex_destroy(pthread_mutex_t *);


extern int pthread_mutex_init(pthread_mutex_t *restrict,
	const pthread_mutexattr_t *restrict);


extern int pthread_mutex_lock(pthread_mutex_t *);


extern int pthread_mutex_trylock(pthread_mutex_t *);


extern int pthread_mutex_unlock(pthread_mutex_t *);


extern int pthread_mutexattr_destroy(pthread_mutexattr_t *);


extern int pthread_mutexattr_init(pthread_mutexattr_t *);


#endif
