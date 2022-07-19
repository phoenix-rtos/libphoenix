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

#ifndef _LIBPHOENIX_PLATFORM_H_
#define _LIBPHOENIX_PLATFORM_H_


#ifdef __cplusplus
extern "C" {
#endif


extern int platformctl(void *);


extern void wdgreload(void);


#ifdef __cplusplus
}
#endif


#endif
