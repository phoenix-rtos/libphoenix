/*
 * Phoenix-RTOS
 *
 * Operating system kernel
 *
 * Interrupt stubs
 *
 * Copyright 2012, 2016 Phoenix Systems
 * Copyright 2001, 2005 Pawel Pisarczyk
 * Author; Pawel Pisarczyk
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#define __ASSEMBLY__

#include "cpu.h"

.text


#define INTRDEF(sym) \
.globl sym; \
.type sym, @function; \
.align 4, 0x90; \
sym:


#define INTRSTUB(intr, func) \
	pushw %ds;\
	pushw %es;\
	pushw %fs;\
	pushw %gs;\
	pushl %eax;\
	pushl %ebx;\
	pushl %ecx;\
	pushl %edx;\
	pushl %ebp;\
	pushl %esi;\
	pushl %edi;\
	pushl %esp;\
	movl $SEL_KDATA, %eax;\
	movw %ax, %ds;\
	movw %ax, %es;\
	movw %ax, %fs;\
	movw %ax, %gs;\
	;\
	/* Call interrupt handler */ ;\
	leal 0(%esp), %eax;\
	pushl %eax;\
	pushl $intr;\
	call func;\
	addl $8, %esp;\
	popl %esp;\
	popl %edi;\
	popl %esi;\
	popl %ebp;\
	popl %edx;\
	popl %ecx;\
	popl %ebx;\
	popl %eax;\
	popw %gs;\
	popw %fs;\
	popw %es;\
	popw %ds;\
	iret;


INTRDEF(_interrupts_irq0)
	INTRSTUB(0, interrupts_dispatchIRQ);
.size _interrupts_irq0, .-_interrupts_irq0

INTRDEF(_interrupts_irq1)
	INTRSTUB(1, interrupts_dispatchIRQ);
.size _interrupts_irq1, .-_interrupts_irq1

INTRDEF(_interrupts_irq2)
	INTRSTUB(2, interrupts_dispatchIRQ);
.size _interrupts_irq2, .-_interrupts_irq2

INTRDEF(_interrupts_irq3)
	INTRSTUB(3, interrupts_dispatchIRQ);
.size _interrupts_irq3, .-_interrupts_irq3

INTRDEF(_interrupts_irq4)
	INTRSTUB(4, interrupts_dispatchIRQ);
.size _interrupts_irq4, .-_interrupts_irq4

INTRDEF(_interrupts_irq5)
	INTRSTUB(5, interrupts_dispatchIRQ);
.size _interrupts_irq5, .-_interrupts_irq5

INTRDEF(_interrupts_irq6)
	INTRSTUB(6, interrupts_dispatchIRQ);
.size _interrupts_irq6, .-_interrupts_irq6

INTRDEF(_interrupts_irq7)
	INTRSTUB(7, interrupts_dispatchIRQ);
.size _interrupts_irq7, .-_interrupts_irq7

INTRDEF(_interrupts_irq8)
	INTRSTUB(8, interrupts_dispatchIRQ);
.size _interrupts_irq8, .-_interrupts_irq8

INTRDEF(_interrupts_irq9)
	INTRSTUB(9, interrupts_dispatchIRQ);
.size _interrupts_irq9, .-_interrupts_irq9

INTRDEF(_interrupts_irq10)
	INTRSTUB(10, interrupts_dispatchIRQ);
.size _interrupts_irq10, .-_interrupts_irq10

INTRDEF(_interrupts_irq11)
	INTRSTUB(11, interrupts_dispatchIRQ);
.size _interrupts_irq11, .-_interrupts_irq11

INTRDEF(_interrupts_irq12)
	INTRSTUB(12, interrupts_dispatchIRQ);
.size _interrupts_irq12, .-_interrupts_irq12

INTRDEF(_interrupts_irq13)
	INTRSTUB(13, interrupts_dispatchIRQ);
.size _interrupts_irq13, .-_interrupts_irq13

INTRDEF(_interrupts_irq14)
	INTRSTUB(14, interrupts_dispatchIRQ);
.size _interrupts_irq14, .-_interrupts_irq14

INTRDEF(_interrupts_irq15)
	INTRSTUB(15, interrupts_dispatchIRQ);
.size _interrupts_irq15, .-_interrupts_irq15

INTRDEF(_interrupts_unexpected)
	INTRSTUB(255, _interrupts_unexpected);
.size _interrupts_unexpected, .-_interrupts_unexpected

INTRDEF(_interrupts_syscall)
	pushw %ds
	pushw %es
	pushw %fs
	pushw %gs
	pushl %eax
	pushl %ebx
	pushl %ecx
	pushl %edx
	pushl %ebp
	pushl %esi
	pushl %edi

	movl $SEL_KDATA, %edx
	movw %dx, %ds
	movw %dx, %es

	call *syscalls(,%eax,4)

	popl %edi
	popl %esi
	popl %ebp
	popl %edx
	popl %ecx
	popl %ebx
	addl $4, %esp
	popw %gs
	popw %fs
	popw %es
	popw %ds

	iret
.size _interrupts_syscall, .-_interrupts_syscall
