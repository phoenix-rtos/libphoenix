/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Architecture dependent part (arch/armv7r)
 *
 * Copyright 2017, 2024, 2026 Phoenix Systems
 * Author: Pawel Pisarczyk, Lukasz Leczkowski, Michal Lach
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_ARCH_ARMV7R_ARCH_H_
#define _LIBPHOENIX_ARCH_ARMV7R_ARCH_H_

#define __ARCH_STDINT <arch/armv7r/stdint.h>
#define __ARCH_LIMITS <arch/armv7r/limits.h>

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
