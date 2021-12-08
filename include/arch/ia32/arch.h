/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Architecture dependent definitions
 *
 * Copyright 2017 Phoenix Systems
 * Author: Pawel Pisarczyk
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_ARCH_IA32_ARCH_H_
#define _LIBPHOENIX_ARCH_IA32_ARCH_H_

#include <phoenix/arch.h>


#define SIZE_PAGE _Pragma("GCC warning \"'SIZE_PAGE' is deprecated. Use _PAGE_SIZE from arch.h or PAGE_SIZE from limits.h (POSIX only)\"") _PAGE_SIZE

#define __MEMCPY
#define __MEMSET


/* Macros for platform dependent symbolic constants and types */
#define __ARCH_STDINT <arch/ia32/stdint.h>
#define __ARCH_LIMITS <arch/ia32/limits.h>
#define __ARCH_SYS_IO <arch/ia32/io.h>


#endif
