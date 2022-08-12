/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Architecture dependent part (arch/armv7)
 *
 * Copyright 2017 Phoenix Systems
 * Author: Pawel Pisarczyk
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_ARCH_ARMV7_ARCH_H_
#define _LIBPHOENIX_ARCH_ARMV7_ARCH_H_

#define __BYTE_ORDER __LITTLE_ENDIAN

#define __ARCH_STDINT <arch/armv7/stdint.h>
#define __ARCH_LIMITS <arch/armv7/limits.h>
#define __ARCH_SYS_TYPES <arch/armv7/types.h>

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

#define _PAGE_SIZE 0x200
#define SIZE_PAGE _Pragma("GCC warning \"'SIZE_PAGE' is deprecated. Use _PAGE_SIZE from arch.h or PAGE_SIZE from limits.h (POSIX only)\"") _PAGE_SIZE

#define __LIBPHOENIX_ARCH_TLS_SUPPORTED

#endif
