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
	// stack: iret data, 16b align, esp, signal number, handler
	xchgl  4(%esp), %eax
	xchgl   (%esp), %ebx
	push %ecx
	push %edx

	sub $8, %esp
	push %eax
	call %ebx
	addl $12, %esp

	pop %edx
	pop %ecx
	pop %ebx
	pop %eax
	pop %esp
	iret
.size _signal_trampoline, .-_signal_trampoline

