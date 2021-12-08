/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * unistd/hostname.c
 *
 * Copyright 2021 Phoenix Systems
 * Author: Ziemowit Leszczynski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <errno.h>
#include <unistd.h>
#include <sys/types.h>


WRAP_ERRNO_DEF(int, gethostname, (char *name, size_t namelen), (name, namelen))
WRAP_ERRNO_DEF(int, sethostname, (const char *name, size_t namelen), (name, namelen))
