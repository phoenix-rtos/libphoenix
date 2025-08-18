/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/threads
 *
 * Copyright 2017, 2018 Phoenix Systems
 * Author: Pawel Pisarczyk, Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_SYS_THREADS_H_
#define _LIBPHOENIX_SYS_THREADS_H_

#include <sys/types.h>
#include <sys/rb.h>
#include <stddef.h>
#include <phoenix/sysinfo.h>
#include <phoenix/signal.h>
#include <phoenix/threads.h>
#include <phoenix/time.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
	handle_t mutex;
	handle_t cond;
	volatile unsigned int v;
} semaphore_t;


struct __errno_t {
	rbnode_t linkage;
	int tid;
	int no;
};


void _errno_new(struct __errno_t *e);


void _errno_remove(struct __errno_t *e);


int gettid(void);


int exec(const char *path, char *const argv[], char *const env[]);


int spawnSyspage(const char *imap, const char *dmap, const char *name, char *const argv[]);


int threadJoin(int tid, time_t timeout);


int beginthreadex(void (*start)(void *), int priority, void *stack, unsigned int stacksz, void *arg, handle_t *id);


__attribute__((noreturn)) void endthread(void);


static inline int beginthread(void (*start)(void *), int priority, void *stack, unsigned int stacksz, void *arg)
{
	return beginthreadex(start, priority, stack, stacksz, arg, NULL);
}


int threadsinfo(int n, threadinfo_t *info);


/* Sets the thread priority to val or retrieves the current thread priority when val == PH_GET_PRIO. Returns the current thread priority in *res if res != NULL. */
int sys_priority(int val, int *res);


int setPriority(int val);


int getPriority(void);


int priority(int val) __attribute__((deprecated("use setPriority() or getPriority() instead")));


int phMutexCreate(handle_t *h, const struct lockAttr *attr);


int mutexCreate(handle_t *h);


static inline int mutexCreateWithAttr(handle_t *h, const struct lockAttr *attr)
{
	return phMutexCreate(h, attr);
}


int phMutexLock(handle_t h);


int mutexLock(handle_t h);


int mutexLock2(handle_t h1, handle_t h2);


int mutexTry(handle_t h);


int mutexConsistent(handle_t h);


/* Sets the prioceiling to val or retrieves the current prioceiling when val == PH_GET_PRIO. Returns the current prioceiling in *res if res != NULL. */
int mutexPrioCeiling(handle_t h, int prioceiling, int *res);


int mutexUnlock(handle_t h);


int semaphoreCreate(semaphore_t *s, unsigned int v);


int semaphoreDown(semaphore_t *s, time_t timeout);


int semaphoreUp(semaphore_t *s);


int semaphoreDone(semaphore_t *s);


int phCondCreate(handle_t *h, const struct condAttr *attr);


int condCreate(handle_t *h);


static inline int condCreateWithAttr(handle_t *h, const struct condAttr *attr)
{
	return phCondCreate(h, attr);
}


int condWait(handle_t h, handle_t m, time_t timeout);


int phCondWait(handle_t h, handle_t m, time_t timeout);


int condSignal(handle_t h);


int condBroadcast(handle_t h);


int resourceDestroy(handle_t h);


int signalAction(int signal, const struct sigaction *act, struct sigaction *oact, void (*trampoline)(void));


int sys_tkill(int pid, int tid, int signal);


int signalReturn(int signal);


unsigned int signalMask(unsigned mask, unsigned mmask);


int signalSuspend(unsigned mask);


#ifdef __cplusplus
}
#endif


#endif
