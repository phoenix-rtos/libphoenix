/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/utsname.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _SYS_UTSNAME_H_
#define _SYS_UTSNAME_H_

#include <errno.h>
#include <phoenix/utsname.h>


#ifdef __cplusplus
extern "C" {
#endif


extern int uname(struct utsname *name);


#ifdef __cplusplus
}
#endif


#endif
