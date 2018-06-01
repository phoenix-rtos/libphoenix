/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/threads
 *
 * Copyright 2017 Phoenix Systems
 * Author: Pawel Pisarczyk
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_SYS_THREADS_H_
#define _LIBPHOENIX_SYS_THREADS_H_

#include ARCH
#include <phoenix/sysinfo.h>
#include <phoenix/signal.h>

extern int beginthreadex(void (*start)(void *), unsigned int priority, void *stack, unsigned int stacksz, void *arg, handle_t *id);


extern void endthread(void);


static inline int beginthread(void (*start)(void *), unsigned int priority, void *stack, unsigned int stacksz, void *arg)
{
	return beginthreadex(start, priority, stack, stacksz, arg, NULL);
}


extern int threadsinfo(int n, threadinfo_t *info);


extern int mutexCreate(handle_t *h);


extern int mutexLock(handle_t h);


extern int mutexTry(handle_t h);


extern int mutexUnlock(handle_t h);


extern int condCreate(handle_t *h);


extern int condWait(handle_t h, handle_t m, time_t timeout);


extern int condSignal(handle_t h);


extern int resourceDestroy(handle_t h);


extern int signalHandle(void (*handler)(int), unsigned mask, unsigned mmask);


extern int signalPost(int pid, int signal);


extern int signalReturn(int signal);


extern int signalMask(unsigned mask, unsigned mmask);

#endif
