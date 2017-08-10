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


static inline int mutex(handle_t *h) { return 0; }


static inline int lock(handle_t h) { return 0; }


static inline int unlock(handle_t h) { return 0; }


extern int beginthread(void (*start)(void *), unsigned int priority, void *stack, void *arg);


extern void endthread(void);


#endif
