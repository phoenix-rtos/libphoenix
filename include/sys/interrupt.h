/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/interrupt
 *
 * Copyright 2017 Phoenix Systems
 * Author: Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_SYS_INTERRUPT_H_
#define _LIBPHOENIX_SYS_INTERRUPT_H_


#ifdef __cplusplus
extern "C" {
#endif


extern int interrupt(unsigned int n, int (*f)(unsigned int, void *), void *arg, unsigned int cond, unsigned int *handle);


#ifdef __cplusplus
}
#endif


#endif
