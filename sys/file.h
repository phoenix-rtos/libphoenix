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

#include ARCH


extern int fileAdd(unsigned int *h, oid_t *oid);


extern int fileSet(unsigned int h, char flags, oid_t *oid, offs_t offs);


extern int fileGet(unsigned int h, char flags, oid_t *oid, offs_t *offs);


extern int fileRemove(unsigned int h);


#endif
