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
	// stack: return address, signal number
	xchgl (%esp), %eax
	push %ebx
	push %ecx
	push %edx
	pushf

	push %eax
	call _signal_handler
	addl $4, %esp

	popf
	pop %edx
	pop %ecx
	pop %ebx
	pop %eax
	ret
.size _signal_trampoline, .-_signal_trampoline

