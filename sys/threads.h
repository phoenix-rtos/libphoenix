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


extern int beginthread(void (*start)(void *), unsigned int priority, void *stack, void *arg);


extern void endthread(void);


extern int mutexCreate(handle_t *h);


extern int mutexLock(handle_t h);


extern int mutexUnlock(handle_t h);


extern int condCreate(handle_t *h);


extern int condWait(handle_t h, handle_t m, time_t timeout);


extern int condSignal(handle_t h);


#endif
