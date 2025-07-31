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

/* number, handler func, func argument, cond handle, result resource handle */
#define interrupt(n, f, arg, c, h) __interrupt((n), (f), (arg), (c), (h))

#ifdef __cplusplus
}
#endif


#endif
