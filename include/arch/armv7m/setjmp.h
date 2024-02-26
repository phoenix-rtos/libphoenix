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

#ifndef _LIBPHOENIX_ARCH_ARMV7M_SETJMP_H_
#define _LIBPHOENIX_ARCH_ARMV7M_SETJMP_H_


#include <phoenix/arch/armv7m/stdtypes.h>


struct __jmp_buf {
	__u32 sigFlg;
	__u32 sigMsk;
	__u32 sp;
	__u32 r[8];
	__u32 lr;

#ifndef __SOFTFP__
	__u64 d[8];
	__u32 fpscr;
#endif
} __attribute__((packed, aligned(8)));


#endif
