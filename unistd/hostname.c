/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * unistd: gethostname(), sethostname()
 *
 * Copyright 2021 Phoenix Systems
 * Author: Ziemowit Leszczynski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <unistd.h>
#include <errno.h>


WRAP_ERRNO_DEF(int, gethostname, (char *name, size_t namelen), (name, namelen))
WRAP_ERRNO_DEF(int, sethostname, (const char *name, size_t namelen), (name, namelen))
