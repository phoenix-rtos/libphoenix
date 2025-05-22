/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Signal trampoline (aarch64)
 *
 * Copyright 2024 Phoenix Systems
 * Author: Jacek Maksymowicz
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
	ldr x0, [sp], 8 /* signal number */
	bl _signal_handler

	ldp x0, x1, [sp], #0x10 /* oldmask, cpu_context_t *  */
	ldp x2, x3, [sp], #0x10 /* pc, sp */
	ldr x4, [sp], #0x10 /* psr, skip padding at the end */
	bl sigreturn
.size _signal_trampoline, .-_signal_trampoline
