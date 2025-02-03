/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/statvfs.h
 *
 * Copyright 2022 Phoenix Systems
 * Author: Lukasz Kosinski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_SYS_STATVFS_H_
#define _LIBPHOENIX_SYS_STATVFS_H_

#include <sys/types.h>
#include <phoenix/posix-statvfs.h>


#ifdef __cplusplus
extern "C" {
#endif


/* Filesystem flags */
#define ST_RDONLY (1 << 0) /* Read-only filesystem */
#define ST_NOSUID (1 << 1) /* Ignore S_ISUID and S_ISGID file mode bits */


extern int statvfs(const char *path, struct statvfs *buf);
extern int fstatvfs(int fildes, struct statvfs *buf);


#ifdef __cplusplus
}
#endif


#endif
