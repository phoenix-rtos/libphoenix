/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/fs
 *
 * Copyright 2017-2018 Phoenix Systems
 * Author: Aleksander Kaminski, Pawel Pisarczyk
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_SYS_FILE_H_
#define _LIBPHOENIX_SYS_FILE_H_

#include <sys/types.h>
#include <phoenix/file.h>


#ifdef __cplusplus
extern "C" {
#endif


enum { otDir = 0, otFile, otDev, otSymlink, otUnknown };


enum { mtMount = 0xf50, mtUmount, mtSync, mtStat, mtMountPoint };


typedef struct {
	oid_t mnt;
	unsigned long mode;
	char fstype[16];
} mount_i_msg_t;


typedef struct {
	oid_t oid;
} mount_o_msg_t;


extern int flock(int fd, int operation);


extern int fdResolve(int fd, oid_t *oid, oid_t *dev);


#ifdef __cplusplus
}
#endif


#endif
