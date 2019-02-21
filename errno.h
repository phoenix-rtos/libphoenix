/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Error definitions
 *
 * Copyright 2017, 2018 Phoenix Systems
 * Author: Pawel Pisarczyk
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_ERRNO_H_
#define _LIBPHOENIX_ERRNO_H_


extern int errno;


#include <phoenix/errno.h>


/* Linux/x86-compatible numbers for easier debugging */

#define ENOTEMPTY       39
#define EUNATCH         49
#define ENODATA         61
#define EPROTO          71
#define EBADMSG         74
#define EMSGSIZE        90
#define ENOPROTOOPT     92
#define EOPNOTSUPP      95
#define EPFNOSUPPORT    96
#define EAFNOSUPPORT    97
#define EADDRINUSE      98
#define EADDRNOTAVAIL   99
#define ENETUNREACH		101
#define ECONNABORTED    103
#define ECONNRESET      104
#define ENOBUFS         105
#define EISCONN         106
#define ENOTCONN        107
#define ETIMEDOUT       110
#define ECONNREFUSED    111
#define EHOSTUNREACH    113
#define EALREADY        114
#define EINPROGRESS     115

#define ENOLCK          116
#define ENOTRECOVERABLE 131
#define ENOTSUP			134


static inline int set_errno(int x)
{
	if (x < 0) {
		errno = -x;
		return -1;
	}

	return x;
}


#define SET_ERRNO set_errno


#define WRAP_ERRNO_DEF(rettype, function, arguments, argnames) \
	extern rettype sys_##function arguments; \
	rettype function arguments \
	{ \
		return SET_ERRNO(sys_##function argnames); \
	}

#endif
