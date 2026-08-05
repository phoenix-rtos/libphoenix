/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/interrupt
 *
 * Copyright 2017, 2024 Phoenix Systems
 * Author: Aleksander Kaminski, Lukasz Leczkowski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_SYS_INTERRUPT_H_
#define _LIBPHOENIX_SYS_INTERRUPT_H_


#include <sys/types.h>
#include <phoenix/time.h>


#ifdef __cplusplus
extern "C" {
#endif


int interrupt(unsigned int n, int (*f)(unsigned int, void *), void *arg, handle_t queue, handle_t *handle);


int interrupt_queueCreate(handle_t *queue, int clock);


int interrupt_queueCreateLocked(handle_t *queue, int clock);


int interrupt_queueWait(handle_t queue, time_t timeout);


int interrupt_queueWaitLocked(handle_t queue, handle_t lock, time_t timeout);


/* Signals the queue. Not intended to be used from interrupt context - only from the thread context. */
int interrupt_queueSignal(handle_t queue);


/* Broadcasts the queue. Not intended to be used from interrupt context - only from the thread context.
 * Broadcasting to multiple threads is possible only if the queue was created with interrupt_queueCreateLocked() function.
 */
int interrupt_queueBroadcast(handle_t queue);


#ifdef __cplusplus
}
#endif


#endif
