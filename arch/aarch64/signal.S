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
	/* Signal number, oldmask - keep stack aligned to 16 */
	ldp x0, x19, [sp], #0x10
	bl _signal_handler

	mov x0, x19             /* move oldmask to x0 */
	ldp x1, x2, [sp], #0x10 /* cpu_context_t *, pc */
	ldp x3, x4, [sp], #0x10 /* sp, psr */
	bl sigreturn
.size _signal_trampoline, .-_signal_trampoline
