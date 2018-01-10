/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * types.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _SYS_TYPES_H
#define _SYS_TYPES_H

#include <stdint.h>


typedef int pid_t;
typedef int off_t;
typedef int mode_t;
typedef int gid_t;
typedef int uid_t;

typedef int ino_t;
typedef int nlink_t;
typedef int blksize_t;
typedef int blkcnt_t;

typedef int suseconds_t;
typedef int clock_t;

typedef uint64_t uintptr_t;

#endif
