/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Signal trampoline (RISCV64)
 *
 * Copyright 2019 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#define __ASSEMBLY__

.text

.globl _signal_trampoline
.type _signal_trampoline, %function
_signal_trampoline:

	// stack: return address, signal number
	addi sp, sp, -136
	sd x1, (sp)          /* ra */
	sd x3, 8(sp)         /* gp */
	sd x5, 16(sp)        /* t0 */
	sd x6, 24(sp)        /* t1 */
	sd x7, 32(sp)        /* t2 */
	sd x10, 40(sp)       /* a0 */
	sd x11, 48(sp)       /* a1 */
	sd x12, 56(sp)       /* a2 */
	sd x13, 64(sp)       /* a3 */
	sd x14, 72(sp)       /* a4 */
	sd x15, 80(sp)       /* a5 */
	sd x16, 88(sp)       /* a6 */
	sd x17, 96(sp)       /* a7 */
	sd x28, 104(sp)      /* t3 */
	sd x29, 112(sp)      /* t4 */
	sd x30, 120(sp)      /* t5 */
	sd x31, 128(sp)      /* t6 */

	ld a0, 136(sp)      /* signal number is 1st argument */
	call _signal_handler

	ld x1, (sp)          /* ra */
	ld x3, 8(sp)         /* gp */
	ld x5, 16(sp)        /* t0 */
	ld x6, 24(sp)        /* t1 */
	ld x7, 32(sp)        /* t2 */
	ld x10, 40(sp)       /* a0 */
	ld x11, 48(sp)       /* a1 */
	ld x12, 56(sp)       /* a2 */
	ld x13, 64(sp)       /* a3 */
	ld x14, 72(sp)       /* a4 */
	ld x15, 80(sp)       /* a5 */
	ld x16, 88(sp)       /* a6 */
	ld x17, 96(sp)       /* a7 */
	ld x28, 104(sp)      /* t3 */
	ld x29, 112(sp)      /* t4 */
	ld x30, 120(sp)      /* t5 */
	ld x31, 128(sp)      /* t6 */
	addi sp, sp, 144     /* saved state, signal number and return address on stack */

	/* TODO correct this, it is wrong, we need a way to load pc without touching any other register */
	ld t4, -8(sp)
	jr t4

.size _signal_trampoline, .-_signal_trampoline
