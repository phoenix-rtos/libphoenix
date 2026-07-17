/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Architecture dependent part
 *
 * Copyright 2017, 2026 Phoenix Systems
 * Author: Pawel Pisarczyk, Michal Lach
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_ARCH_IA32_ARCH_H_
#define _LIBPHOENIX_ARCH_IA32_ARCH_H_

#define __ARCH_STDINT <arch/ia32/stdint.h>
#define __ARCH_LIMITS <arch/ia32/limits.h>
#define __ARCH_SYS_IO <arch/ia32/io.h>

#define __MEMCPY
#define __MEMSET

#define __LIBPHOENIX_ARCH_TLS_SUPPORTED

#endif
