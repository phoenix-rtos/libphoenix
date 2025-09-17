/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/termios.h
 *
 * Copyright 2025 Phoenix Systems
 * Author: Grzegorz Mazurczak
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _SYS_TERMIOS_H_
#define _SYS_TERMIOS_H_

#include <sys/types.h>


#ifdef __cplusplus
extern "C" {
#endif

/* Phoenix-RTOS custom termios API function declarations*/

/* Sets non-standard baudrate */
int tcSetSpeed(int fd, int speed);


#ifdef __cplusplus
}
#endif

#endif
