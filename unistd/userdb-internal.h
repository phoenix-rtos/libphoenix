/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Internal user/group database limits
 *
 * Copyright 2026 Phoenix Systems
 * Author: Adam Greloch
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _LIBPHOENIX_INTERNAL_USERDB_H_
#define _LIBPHOENIX_INTERNAL_USERDB_H_


#include <limits.h>


/* Largest buffer getpw*_r() may need, i.e. the longest /etc/passwd entry we accept. */
#define PWD_MAX_BUFSIZE (NAME_MAX + 128 /* pw_passwd */ + 128 /* pw_gecos */ + PATH_MAX + PATH_MAX)

/* Largest buffer getgr*_r() may need. */
#define GRP_MAX_BUFSIZE 2048

#define GRP_DEFAULT_BUFSIZE 128


#endif
