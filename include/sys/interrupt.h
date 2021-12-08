/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/interrupt.h
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

#include <sys/types.h>


extern int interrupt(unsigned int n, int (*f)(unsigned int, void *), void *arg, handle_t cond, handle_t *handle);


#endif
