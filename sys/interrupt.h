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

extern int interrupt(unsigned int n, int (*f)(unsigned int, void *), void *arg);

#endif
