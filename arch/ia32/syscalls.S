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

#define SYSCALLNUM(sym) syscalls_num_ ## sym

#define SYSCALLDEF(sym, sn) \
.equ SYSCALLNUM(sym), sn; \
.globl sym; \
.type sym, @function; \
.align 4, 0x90; \
sym: \
	.cfi_startproc; \
	mov $SYSCALLNUM(sym), %eax; \
	int $0x80; \
	ret; \
	.cfi_endproc; \
.size sym, .-sym


.globl vfork
.type vfork, @function
.align 4, 0x90
vfork:
	/* Save the return address as child will override parent's stack.
	 * Syscalls restore all registers but %eax thus %edx will not be overwritten.
	 * %edx is used as it need not to be restored before function exit. */
	mov (%esp), %edx
	mov $SYSCALLNUM(vforksvc), %eax
	int $0x80
	mov %edx, (%esp)
	ret
.size vfork, .-vfork


#define SYSCALLS_LIBC(name) \
	SYSCALLDEF(name, __COUNTER__);


SYSCALLS(SYSCALLS_LIBC)
