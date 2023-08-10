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
	/* Get signal number from stack */
	lw a0, (sp)
	call _signal_handler

	/* Old signal mask in a0 */
	ld a1, 8(sp)  /* cpu context * */
	ld a2, 16(sp) /* sepc */
	ld a3, 24(sp) /* sp */
	addi sp, sp, 32
	call sigreturn
.size _signal_trampoline, .-_signal_trampoline
