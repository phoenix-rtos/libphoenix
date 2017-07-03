/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * unistd.h
 *
 * Copyright 2017 Phoenix Systems
 * Author: Pawel Pisarczyk
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_UNISTD_H_
#define _LIBPHOENIX_UNISTD_H_

#include ARCH


extern int vfork(void);


extern int usleep(useconds_t usecs);


#endif
