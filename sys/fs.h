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

typedef struct {
	unsigned int id;
	size_t pos;
	unsigned char buff[];
} __attribute__((packed)) fsdata_t;


typedef struct {
	unsigned int mode;
	char name[];
} __attribute__((packed)) fsopen_t;


typedef unsigned int fsclose_t;


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
	unsigned int id;
	unsigned int cmd;
	unsigned long arg;
} __attribute__((packed)) fsfcntl_t;

#endif
