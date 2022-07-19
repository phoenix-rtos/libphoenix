/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/sysmacros
 *
 * Copyright 2018 Phoenix Systems
 * Author: Marek Bialowas
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _SYS_SYSMACROS_H_
#define _SYS_SYSMACROS_H_

#include <sys/types.h>


#ifdef __cplusplus
extern "C" {
#endif


/* NOTE: this can/should be macros */

dev_t makedev(unsigned int maj, unsigned int min);
unsigned int major(dev_t dev);
unsigned int minor(dev_t dev);


#ifdef __cplusplus
}
#endif


#endif //_SYS_SYSMACROS_H_
