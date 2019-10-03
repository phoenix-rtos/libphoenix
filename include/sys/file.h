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

#include <arch.h>


enum { otDir = 0, otFile, otDev, otSymlink, otUnknown };


enum { atMode = 0, atUid, atGid, atSize, atType, atPort, atPollStatus, atEventMask, atCTime, atMTime, atATime, atLinks, atDev };


enum { mtMount = 0xf50, mtUmount, mtSync };


typedef struct {
	long id;
	unsigned mode;
	char fstype[16];
} mount_msg_t;


extern int fileAdd(unsigned int *h, oid_t *oid, unsigned mode);


extern int fileSet(unsigned int h, char flags, oid_t *oid, offs_t offs, unsigned mode);


extern int fileGet(unsigned int h, char flags, oid_t *oid, offs_t *offs, unsigned *mode);


extern int fileRemove(unsigned int h);


extern int flock(int fd, int operation);


#endif
