/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/times.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _SYS_TIMES_H_
#define _SYS_TIMES_H_


#include <sys/types.h>


#ifdef __cplusplus
extern "C" {
#endif


struct tms {
	clock_t tms_utime;
	clock_t tms_stime;
	clock_t tms_cutime;
	clock_t tms_cstime;
};


extern clock_t times(struct tms *buffer);


#ifdef __cplusplus
}
#endif


#endif
