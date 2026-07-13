/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * pthread
 *
 * Copyright 2017-2018, 2019 Phoenix Systems
 * Author: Pawel Pisarczyk, Michal Miroslaw, Marcin Baran
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
#include <time.h>
#include <signal.h>


#ifdef __cplusplus
extern "C" {
#endif


#define PTHREAD_CREATE_JOINABLE 0
#define PTHREAD_CREATE_DETACHED 1

#define PTHREAD_EXPLICIT_SCHED 0
#define PTHREAD_INHERIT_SCHED  1

#define PTHREAD_PRIO_NONE    0
#define PTHREAD_PRIO_INHERIT 1
#define PTHREAD_PRIO_PROTECT 2

#define PTHREAD_PROCESS_SHARED  0
#define PTHREAD_PROCESS_PRIVATE 1

#define PTHREAD_SCOPE_PROCESS 0
#define PTHREAD_SCOPE_SYSTEM  1

#define PTHREAD_MUTEX_INITIALIZER { 0, 0 }

#define PTHREAD_MUTEX_NORMAL     PH_LOCK_NORMAL
#define PTHREAD_MUTEX_ERRORCHECK PH_LOCK_ERRORCHECK
#define PTHREAD_MUTEX_RECURSIVE  PH_LOCK_RECURSIVE
#define PTHREAD_MUTEX_DEFAULT    PTHREAD_MUTEX_NORMAL

#define PTHREAD_ONCE_INIT 1

#define PTHREAD_CANCEL_DISABLE 0
#define PTHREAD_CANCEL_ENABLE  1
#define PTHREAD_CANCELED       2

#define PTHREAD_COND_INITIALIZER { 0, 0 }


int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void *), void *arg);


int pthread_join(pthread_t thread, void **value_ptr);


int pthread_detach(pthread_t thread);


int pthread_setcancelstate(int state, int *oldstate);


int pthread_cancel(pthread_t thread);


void pthread_testcancel(void);


pthread_t pthread_self(void);


int pthread_equal(pthread_t t1, pthread_t t2);


void pthread_exit(void *value_ptr);


int pthread_attr_init(pthread_attr_t *attr);


int pthread_attr_destroy(pthread_attr_t *attr);


int pthread_attr_setstackaddr(pthread_attr_t *attr, void *stackaddr);


int pthread_attr_getstackaddr(const pthread_attr_t *attr, void **stackaddr);


int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize);


int pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stacksize);


int pthread_attr_setstack(pthread_attr_t *attr, void *stackaddr, size_t stacksize);


int pthread_attr_getstack(const pthread_attr_t *attr, void **stackaddr, size_t *stacksize);


int pthread_attr_setschedparam(pthread_attr_t *attr, const struct sched_param *param);


int pthread_attr_getschedparam(const pthread_attr_t *attr, struct sched_param *param);


int pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy);


int pthread_attr_getschedpolicy(const pthread_attr_t *attr, int *policy);


int pthread_attr_setscope(pthread_attr_t *attr, int scope);


int pthread_attr_getscope(const pthread_attr_t *attr, int *scope);


int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate);


int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate);


int pthread_attr_setguardsize(pthread_attr_t *attr, size_t guardsize);


int pthread_attr_getguardsize(const pthread_attr_t *__restrict attr, size_t *__restrict guardsize);


int pthread_attr_getinheritsched(const pthread_attr_t *__restrict attr, int *__restrict inheritsched);


int pthread_attr_setinheritsched(pthread_attr_t *attr, int inheritsched);


int pthread_setschedprio(pthread_t thread, int prio);


int pthread_getschedparam(pthread_t thread, int *policy, struct sched_param *__restrict param);


int pthread_setschedparam(pthread_t thread, int policy, const struct sched_param *param);


int pthread_mutex_destroy(pthread_mutex_t *);


int pthread_mutex_init(pthread_mutex_t *__restrict, const pthread_mutexattr_t *__restrict);


int pthread_mutex_lock(pthread_mutex_t *);


int pthread_mutex_trylock(pthread_mutex_t *);


int pthread_mutex_unlock(pthread_mutex_t *);


int pthread_mutexattr_destroy(pthread_mutexattr_t *attr);


int pthread_mutexattr_init(pthread_mutexattr_t *attr);


int pthread_mutexattr_gettype(const pthread_mutexattr_t *attr, int *type);


int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type);


int pthread_condattr_init(pthread_condattr_t *attr);


int pthread_condattr_destroy(pthread_condattr_t *attr);


int pthread_condattr_setpshared(pthread_condattr_t *attr, int pshared);


int pthread_condattr_getpshared(const pthread_condattr_t *__restrict attr, int *__restrict pshared);


int pthread_condattr_setclock(pthread_condattr_t *attr, clockid_t clock_id);


int pthread_condattr_getclock(const pthread_condattr_t *__restrict attr, clockid_t *__restrict clock_id);


int pthread_cond_init(pthread_cond_t *__restrict cond, const pthread_condattr_t *__restrict attr);


int pthread_cond_destroy(pthread_cond_t *cond);


int pthread_cond_signal(pthread_cond_t *cond);


int pthread_cond_broadcast(pthread_cond_t *cond);


int pthread_cond_wait(pthread_cond_t *__restrict cond, pthread_mutex_t *__restrict mutex);


int pthread_cond_timedwait(pthread_cond_t *__restrict cond,
		pthread_mutex_t *__restrict mutex,
		const struct timespec *__restrict abstime);


int pthread_sigmask(int how, const sigset_t *__restrict__ set, sigset_t *__restrict__ oset);


int pthread_kill(pthread_t thread, int sig);


int pthread_key_create(pthread_key_t *key, void (*destructor)(void *));


int pthread_key_delete(pthread_key_t key);


int pthread_setspecific(pthread_key_t key, const void *value);


void *pthread_getspecific(pthread_key_t key);


int pthread_once(pthread_once_t *once_control, void (*init_routine)(void));


int pthread_atfork(void (*prepare)(void), void (*parent)(void), void (*child)(void));


void _pthread_atfork_prepare(void);


void _pthread_atfork_parent(void);


void _pthread_atfork_child(void);


void pthread_cleanup_push(void (*routine)(void *), void *arg);


void pthread_cleanup_pop(int execute);


int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);


int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock);


int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock);


int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);


int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);


int pthread_rwlock_destroy(pthread_rwlock_t *rwlock);


int pthread_rwlock_init(pthread_rwlock_t *__restrict__ rwlock, const pthread_rwlockattr_t *__restrict__ attr);


#ifdef __cplusplus
}
#endif


#endif
