/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * arch-dependent part of setjmp.h
 *
 * Copyright 2024 Phoenix Systems
 * Author: Lukasz Leczkowski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_ARCH_RISCV64_SETJMP_H_
#define _LIBPHOENIX_ARCH_RISCV64_SETJMP_H_


#include <phoenix/arch/riscv64/stdtypes.h>


struct __jmp_buf {
	__u64 s[12];
	__u64 sp;
	__u64 ra;
	__u64 sigFlg;
	__u64 sigMsk;
	__u64 f[12];
	__u64 fcsr;
} __attribute__((packed, aligned(8)));


#endif
