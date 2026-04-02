/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * ar.h - archive file header
 *
 * Copyright 2026 Phoenix Systems
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_AR_H_
#define _LIBPHOENIX_AR_H_

/* Archive magic string */
#define ARMAG  "!<arch>\n"
#define SARMAG 8

/* Archive file member header */
#define ARFMAG "`\n"

struct ar_hdr {
	char ar_name[16]; /* Member file name */
	char ar_date[12]; /* File date, decimal seconds since epoch */
	char ar_uid[6];   /* User ID, decimal */
	char ar_gid[6];   /* Group ID, decimal */
	char ar_mode[8];  /* File mode, octal */
	char ar_size[10]; /* File size, decimal */
	char ar_fmag[2];  /* Always ARFMAG */
};

#endif /* _LIBPHOENIX_AR_H_ */
