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


extern void _errno_new(struct __errno_t *e);


extern void _errno_remove(struct __errno_t *e);


extern int perf_start(unsigned pid);


extern int perf_read(void *buffer, size_t bufsz);


extern int perf_finish(void);


extern int gettid(void);


extern int exec(const char *path, char *const argv[], char *const env[]);


extern int spawnSyspage(const char *imap, const char *dmap, const char *name, char *const argv[]);


extern int threadJoin(int tid, time_t timeout);


extern int beginthreadex(void (*start)(void *), unsigned int priority, void *stack, unsigned int stacksz, void *arg, handle_t *id);


__attribute__((noreturn))
extern void endthread(void);


static inline int beginthread(void (*start)(void *), unsigned int priority, void *stack, unsigned int stacksz, void *arg)
{
	return beginthreadex(start, priority, stack, stacksz, arg, NULL);
}


extern int threadsinfo(int n, threadinfo_t *info);


extern int priority(int priority);


extern int mutexCreate(handle_t *h);


extern int phMutexLock(handle_t h);


extern int mutexLock(handle_t h);


extern int mutexLock2(handle_t h1, handle_t h2);


extern int mutexTry(handle_t h);


extern int mutexUnlock(handle_t h);


extern int semaphoreCreate(semaphore_t *s, unsigned int v);


extern int semaphoreDown(semaphore_t *s, time_t timeout);


extern int semaphoreUp(semaphore_t *s);


extern int semaphoreDone(semaphore_t *s);


extern int condCreate(handle_t *h);


extern int condWait(handle_t h, handle_t m, time_t timeout);


extern int phCondWait(handle_t h, handle_t m, time_t timeout);


extern int condSignal(handle_t h);


extern int condBroadcast(handle_t h);


extern int resourceDestroy(handle_t h);


extern int signalHandle(void (*handler)(void), unsigned mask, unsigned mmask);


extern int signalPost(int pid, int tid, int signal);


extern int signalReturn(int signal);


extern unsigned int signalMask(unsigned mask, unsigned mmask);


extern int signalSuspend(unsigned mask);


#ifdef __cplusplus
}
#endif


#endif
