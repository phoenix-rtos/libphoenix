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

#ifndef _LIBPHOENIX_ARCH_SPARCV8LEON3_SETJMP_H_
#define _LIBPHOENIX_ARCH_SPARCV8LEON3_SETJMP_H_


#include <phoenix/arch/sparcv8leon3/stdtypes.h>


struct __jmp_buf {
	__u32 sp;
	__u32 o7;
	__u32 i[2];
	__u32 sigFlg;
	__u32 sigMsk;
} __attribute__((packed, aligned(8)));


#endif
