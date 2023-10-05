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


#ifdef __cplusplus
extern "C" {
#endif


/* NOTE: these values can't be changed as they are kept on FLASH (eg. in jffs2) */
#define DT_UNKNOWN 0
#define DT_FIFO    1
#define DT_CHR     2
#define DT_DIR     4
#define DT_BLK     6
#define DT_REG     8
#define DT_LNK     10
#define DT_SOCK    12
#define DT_WHT     14


struct dirent {
	ino_t  d_ino;
	uint32_t d_type;
	uint16_t d_reclen;
	uint16_t d_namlen;
	char   d_name[];
};


extern struct dirent *readdir(DIR *s);


extern DIR *opendir(const char *dirname);


extern void rewinddir(DIR *dirp);


extern int closedir(DIR *dirp);


#ifdef __cplusplus
}
#endif


#endif
