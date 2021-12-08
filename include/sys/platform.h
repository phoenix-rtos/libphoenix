/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/platform
 *
 * Copyright 2017 Phoenix Systems
 * Author: Jan Sikorski, Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_SYS_PLATFORM_H_
#define _LIBPHOENIX_SYS_PLATFORM_H_

#include <phoenix/pctl.h>


extern int platformctl(void *);


extern void wdgreload(void);


#endif
