/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Architecture dependent part (arch/armv7a)
 *
 * Copyright 2017, 2018, 2026 Phoenix Systems
 * Author: Pawel Pisarczyk, Aleksander Kaminski, Michal Lach
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_ARCH_ARMV7A_ARCH_H_
#define _LIBPHOENIX_ARCH_ARMV7A_ARCH_H_

#define __ARCH_STDINT <arch/armv7a/stdint.h>
#define __ARCH_LIMITS <arch/armv7a/limits.h>

#define __MEMCPY
#define __MEMCMP
#define __MEMSET
#define __STRLEN
#define __STRNLEN
#define __STRCMP
#define __STRNCMP
#define __STRCPY
#define __STRNCPY
#define __MEMMOVE

#define __LIBPHOENIX_ARCH_TLS_SUPPORTED

#endif
