/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Architecture dependent part (arch/aarch64)
 *
 * Copyright 2017, 2018, 2026 Phoenix Systems
 * Author: Pawel Pisarczyk, Aleksander Kaminski, Michal Lach
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_ARCH_AARCH64_ARCH_H_
#define _LIBPHOENIX_ARCH_AARCH64_ARCH_H_

#define __ARCH_STDINT <arch/aarch64/stdint.h>
#define __ARCH_LIMITS <arch/aarch64/limits.h>

#define __MEMCPY
#define __MEMSET
#define __MEMMOVE

#define __LIBPHOENIX_ARCH_TLS_SUPPORTED

#endif
