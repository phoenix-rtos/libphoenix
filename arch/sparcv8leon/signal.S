/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Signal trampoline (sparcv8leon)
 *
 * Copyright 2022 Phoenix Systems
 * Author: Lukasz Leczkowski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */


#define __ASSEMBLY__


.section ".text"
.align 4

.globl _signal_trampoline
.type _signal_trampoline, #function
_signal_trampoline:
	save %sp, -0x60, %sp
	add %sp, 0xc0, %i0

	ld [%i0 + 0x04], %o1 /* handler address */
	jmpl %o1 + 0, %o7
	ld [%i0], %o0 /* signal number */

	ld [%i0 + 0x08], %o0 /* old signal mask */
	ld [%i0 + 0x0c], %o1 /* cpu_context * */
	ld [%i0 + 0x10], %o2 /* pc */
	ld [%i0 + 0x14], %o3 /* npc */
	ld [%i0 + 0x18], %o4 /* sp */
	call sigreturn
	ld [%i0 + 0x1c], %o5 /* psr */
.size _signal_trampoline, .-_signal_trampoline
