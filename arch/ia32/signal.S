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
	/* Signal number on stack */
	call _signal_handler

	addl $4, %esp
	/* Put old mask on stack */
	push %eax
	/* cpu context *, eip, esp on stack */
	call sigreturn
.size _signal_trampoline, .-_signal_trampoline

