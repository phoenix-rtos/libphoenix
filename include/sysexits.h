/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sysexits.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_SYSEXITS_H_
#define _LIBPHOENIX_SYSEXITS_H_


#define EX_OK    0
#define EX__BASE 64


enum { EX_USAGE = EX__BASE, EX_DATAERR, EX_NOINPUT, EX_NOUSER, EX_NOHOST,
	EX_UNAVAILABLE, EX_SOFTWARE, EX_OSERR, EX_OSFILE, EX_CANTCREAT, EX_IOERR,
	EX_TEMPFAIL, EX_PROTOCOL, EX_NOPERM, EX_CONFIG };


#endif
