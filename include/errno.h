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


#include <phoenix/errno.h>


#ifdef __cplusplus
extern "C" {
#endif


/* Linux/x86-compatible numbers for easier debugging */

#define ENOTEMPTY       39
#define ELOOP           40
#define ENOMSG          42
#define EUNATCH         49
#define ENODATA         61
#define ENONET          64
#define EBADFD          66
#define EPROTO          71
#define EBADMSG         74
#define EOVERFLOW       75
#define EILSEQ          84
#define EDESTADDRREQ    89
#define EMSGSIZE        90
#define EPFNOSUPPORT    96
#define EADDRNOTAVAIL   99
#define ENETDOWN        100
#define ENETUNREACH     101
#define ENETRESET       102
#define ECONNABORTED    103
#define ECONNRESET      104
#define ENOBUFS         105
#define ETIMEDOUT       110
#define EHOSTDOWN       112
#define EHOSTUNREACH    113
#define EALREADY        114
#define EINPROGRESS     115
#define ENOLCK          116
#define EUCLEAN         117
#define ENOTRECOVERABLE 131
#define ENOTSUP         EOPNOTSUPP


#define errno (*__errno_location())


extern int *__errno_location(void);


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


#ifdef __cplusplus
}
#endif


#endif
