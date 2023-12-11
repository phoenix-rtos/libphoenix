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


#ifdef __cplusplus
extern "C" {
#endif


enum { otDir = 0, otFile, otDev, otSymlink, otUnknown };


enum { atMode = 0, atUid, atGid, atSize, atBlocks, atIOBlock, atType, atPort, atPollStatus, atEventMask, atCTime, atMTime, atATime, atLinks, atDev };


enum { mtMount = 0xf50, mtUmount, mtSync, mtStat, mtMountPoint };


typedef struct {
	oid_t dev;
	oid_t mnt;
	unsigned long mode;
	char fstype[16];
} mount_i_msg_t;


typedef struct {
	int err;
	oid_t oid;
} mount_o_msg_t;


extern int flock(int fd, int operation);


#ifdef __cplusplus
}
#endif


#endif
