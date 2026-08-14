/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Signal trampoline (ia32)
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
	pop %ebx /* handler */
	/* Signal number on stack */
	call %ebx

	addl $4, %esp
	/* oldmask is now on top of the stack */
	/* cpu context *, eip, esp on stack */
	call sigreturn
.size _signal_trampoline, .-_signal_trampoline

