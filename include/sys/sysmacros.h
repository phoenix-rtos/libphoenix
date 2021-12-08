/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/sysmacros.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Marek Białowąs
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_SYS_SYSMACROS_H_
#define _LIBPHOENIX_SYS_SYSMACROS_H_

#include <sys/types.h>

/* NOTE: this can/should be macros */

extern dev_t makedev(unsigned int maj, unsigned int min);


extern unsigned int major(dev_t dev);


extern unsigned int minor(dev_t dev);


#endif
