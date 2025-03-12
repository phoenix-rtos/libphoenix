/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/utsname.c
 *
 * Copyright 2025 Phoenix Systems
 * Author: Hubert Badocha
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <errno.h>
#include <sys/utsname.h>


WRAP_ERRNO_DEF(int, uname, (struct utsname * name), (name))
