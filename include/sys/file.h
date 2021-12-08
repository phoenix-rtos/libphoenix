/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/file.h
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
#include <phoenix/posix/file.h>


extern int fileAdd(unsigned int *h, oid_t *oid, unsigned int mode);


extern int fileSet(unsigned int h, char flags, oid_t *oid, off_t offs, unsigned int mode);


extern int fileGet(unsigned int h, char flags, oid_t *oid, off_t *offs, unsigned int *mode);


extern int fileRemove(unsigned int h);


extern int flock(int fd, int operation);


#endif
