/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Architecture dependent part of types (arch/ia32)
 *
 * Copyright 2019 Phoenix Systems
 * Author: Andrzej Glowinski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_ARCH_IA32_TYPES_H_
#define _LIBPHOENIX_ARCH_IA32_TYPES_H_

typedef unsigned int addr_t;
typedef long long offs_t;

typedef int ssize_t;
typedef unsigned long long time_t;
typedef unsigned int useconds_t;
typedef int suseconds_t;

typedef unsigned long long id_t;
typedef struct _oid_t {
	unsigned int port;
	id_t id;
} oid_t;

typedef unsigned int handle_t;


#endif
