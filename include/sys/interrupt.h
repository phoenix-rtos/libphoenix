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


#ifdef __cplusplus
extern "C" {
#endif


extern int interrupt(unsigned int n, int (*f)(unsigned int, void *), void *arg, handle_t cond, handle_t *handle);


#ifdef __cplusplus
}
#endif


#endif
