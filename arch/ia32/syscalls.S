/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * syscalls (ia32)
 *
 * Copyright 2017 Phoenix Systems
 * Author; Pawel Pisarczyk
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#define __ASSEMBLY__

#include <phoenix/syscalls.h>

.text


#define SYSCALLDEF(sym, sn) \
.globl sym; \
.type sym, @function; \
.align 4, 0x90; \
sym: \
	.cfi_startproc; \
	movl $sn, %eax; \
	int $0x80; \
	ret; \
	.cfi_endproc; \
.size sym, .-sym


.bss
vfork_address:
.space 4
.text
.globl vfork
.type vfork, @function
.align 4, 0x90
vfork:
	mov (%esp), %eax
	mov %eax, (vfork_address)
	movl $4, %eax
	int $0x80
	cmp $0, %eax
	jz vfork_child
	push %eax
	mov (vfork_address), %eax
	mov %eax, 4(%esp)
	pop %eax
	ret
vfork_child:
	mov $0, %eax
	ret
.size vfork, .-vfork


#define SYSCALLS_LIBC(name) \
	SYSCALLDEF(name, __COUNTER__);


SYSCALLS(SYSCALLS_LIBC)
