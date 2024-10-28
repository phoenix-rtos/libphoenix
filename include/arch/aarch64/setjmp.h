/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * arch-dependent part of setjmp.h
 *
 * Copyright 2024 Phoenix Systems
 * Author: Jacek Maksymowicz
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_ARCH_AARCH64_SETJMP_H_
#define _LIBPHOENIX_ARCH_AARCH64_SETJMP_H_


#include <phoenix/arch/aarch64/stdtypes.h>


struct __jmp_buf {
	__u64 calleeSavedRegs[12];
	__u64 sp;
	__u32 signalFlag;
	__u32 signalMask;

#ifndef __SOFTFP__
	__u64 fpcr;
	__u64 fpsr;
	__u64 calleeSavedFPU[8];
#endif
} __attribute__((packed, aligned(16)));


#endif
