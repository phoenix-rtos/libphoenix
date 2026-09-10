/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Architecture dependent part (arch/armv8m)
 *
 * Copyright 2017, 2024, 2026 Phoenix Systems
 * Author: Pawel Pisarczyk, Aleksander Kaminski, Michal Lach
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_ARCH_ARMV8M_ARCH_H_
#define _LIBPHOENIX_ARCH_ARMV8M_ARCH_H_

#define __ARCH_STDINT <arch/armv8m/stdint.h>
#define __ARCH_LIMITS <arch/armv8m/limits.h>

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

/* FIXME provide libphoenix config to be able to
 * selectively disable/enable features on per
 * project basis.
 * Disabled for now as TLS consumes too much
 * memory to be advantageous on some targets. */
// #define __LIBPHOENIX_ARCH_TLS_SUPPORTED

#endif
