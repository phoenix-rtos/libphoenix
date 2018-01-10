/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * grp.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_GRP_H_
#define _LIBPHOENIX_GRP_H_

#include <sys/types.h>


struct group {
	char   *gr_name;
	gid_t   gr_gid;
	char  **gr_mem;
};


#endif
