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


#ifdef __cplusplus
extern "C" {
#endif


/* Filesystem flags */
#define ST_RDONLY (1 << 0) /* Read-only filesystem */
#define ST_NOSUID (1 << 1) /* Ignore S_ISUID and S_ISGID file mode bits */


struct statvfs
{
	unsigned long f_bsize;   /* Filesystem block size */
	unsigned long f_frsize;  /* Fundamental filesystem block size */
	fsblkcnt_t f_blocks;     /* Number of blocks on filesystem (in f_frsize units) */
	fsblkcnt_t f_bfree;      /* Number of free blocks */
	fsblkcnt_t f_bavail;     /* Number of free blocks available to non-privileged process */
	fsfilcnt_t f_files;      /* Number of inodes */
	fsfilcnt_t f_ffree;      /* Number of free inodes */
	fsfilcnt_t f_favail;     /* Number of free inodes available to non-privileged process */
	unsigned long f_fsid;    /* Filesystem ID */
	unsigned long f_flag;    /* Filesystem flags */
	unsigned long f_namemax; /* Maximum filename length */
};


extern int statvfs(const char *path, struct statvfs *buf);


#ifdef __cplusplus
}
#endif


#endif
