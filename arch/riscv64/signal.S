/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Signal trampoline (RISCV64)
 *
 * Copyright 2019, 2023 Phoenix Systems
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
	/* Get signal number and handler address from stack */
	lw a0, (sp)
	ld a1, 8(sp)
	jalr ra, a1, 0

	lw a0, 16(sp)  /* old signal mask */
	ld a1, 24(sp) /* cpu context * */
	ld a2, 32(sp) /* sepc */
	ld a3, 40(sp) /* sp */
	addi sp, sp, 48
	call sigreturn
.size _signal_trampoline, .-_signal_trampoline
