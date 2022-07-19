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

#include <limits.h>
#include <string.h>
#include <unistd.h>


#ifdef __cplusplus
extern "C" {
#endif


struct utsname {
	char sysname[16];
	char nodename[HOST_NAME_MAX + 1];
	char release[16];
	char version[16];
	char machine[16];
};


static inline int uname(struct utsname *name)
{
	memset(name, 0, sizeof(*name));
	return gethostname(name->nodename, sizeof(name->nodename));
}


#ifdef __cplusplus
}
#endif


#endif
