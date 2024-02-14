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

#ifndef _LIBPHOENIX_ARCH_IA32_SETJMP_H_
#define _LIBPHOENIX_ARCH_IA32_SETJMP_H_


#include <phoenix/arch/ia32/stdtypes.h>


struct __jmp_buf {
	__u32 r[4];
	__u32 sp;
	__u32 jmp;
	__u32 ret;
	__u32 sigMask;
} __attribute__((packed));


#endif
