/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/fs
 *
 * Copyright 2017 Phoenix Systems
 * Author: Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_SYS_FS_H_
#define _LIBPHOENIX_SYS_FS_H_

#include ARCH
#include <fcntl.h>

typedef struct {
	id_t id;
	size_t pos;
	unsigned char buff[];
} __attribute__((packed)) fsdata_t;


typedef struct {
	id_t id;
	mode_t mode;
} __attribute__((packed)) fsopen_t;


typedef id_t fsclose_t;


typedef struct {
	mode_t mode;
	char path[];
} __attribute__((packed)) fscreat_t;


typedef struct {
	unsigned int port;
	char name[];
} __attribute__((packed)) fsmount_t;


typedef struct {
	oid_t oid;
	size_t pos;
	char path[];
} __attribute__((packed)) fslookup_t;


typedef struct {
	id_t id;
	unsigned int cmd;
	unsigned long arg;
} __attribute__((packed)) fsfcntl_t;

#endif
