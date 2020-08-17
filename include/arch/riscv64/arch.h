/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Architecture dependent part (RISCV64)
 *
 * Copyright 2018 Phoenix Systems
 * Author: Pawel Pisarczyk
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_ARCH_RISCV64_ARCH_H_
#define _LIBPHOENIX_ARCH_RISCV64_ARCH_H_

#define __BYTE_ORDER __LITTLE_ENDIAN

#define __ARCH_STDINT <arch/riscv64/stdint.h>
#define __ARCH_LIMITS <arch/riscv64/limits.h>
#define __ARCH_SYS_TYPES <arch/riscv64/types.h>
#define __ARCH_SYS_IO <arch/riscv64/io.h>

#define __MEMCPY
#define __MEMSET

#define _PAGE_SIZE 0x1000
#define SIZE_PAGE _Pragma("GCC warning \"'SIZE_PAGE' is deprecated. Use _PAGE_SIZE from arch.h or PAGE_SIZE from limits.h (POSIX only)\"") _PAGE_SIZE

#endif
