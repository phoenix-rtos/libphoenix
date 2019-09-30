/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * dirent.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_DIRENT_H_
#define _LIBPHOENIX_DIRENT_H_

#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>

#define NAME_MAX 128

/* dirent types */
enum {	dtDir = 0,
		dtFile,
		dtDev,
		dtUnknown
};

struct dirent {
	ino_t  d_ino;
	uint32_t d_type;
	uint16_t d_reclen;
	uint16_t d_namlen;
	char   d_name[];
};


extern struct dirent *readdir(DIR *s);


extern DIR *opendir(const char *dirname);


extern int closedir(DIR *dirp);



#endif
