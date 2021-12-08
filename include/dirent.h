/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * dirent.h
 *
 * Copyright 2018, 2021 Phoenix Systems
 * Author: Jan Sikorski, Lukasz Kosinski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_DIRENT_H_
#define _LIBPHOENIX_DIRENT_H_

#include <stdint.h>
#include <sys/types.h>


/* Directory entry type */
enum {
	dtDir,     /* Directory */
	dtFile,    /* Regular file */
	dtDev,     /* Device file */
	dtSymlink, /* Symbolic link */
	dtUnknown  /* Unknown file type */
};


/* Directory entry */
struct dirent {
	ino_t  d_ino;      /* File serial number */
	uint32_t d_type;   /* Entry type */
	uint16_t d_reclen; /* Entry size in bytes */
	uint16_t d_namlen; /* Filename string or entry length (not including the terminating null) */
	char d_name[];     /* Filename string or entry */
};


/* Directory stream */
typedef struct {
	oid_t oid;             /* Stream oid */
	size_t pos;            /* Stream position */
	struct dirent *dirent; /* Stream entries */
} DIR;


extern struct dirent *readdir(DIR *s);


extern DIR *opendir(const char *dirname);


extern int closedir(DIR *dirp);


#endif
