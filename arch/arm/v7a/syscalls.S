/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * syscalls (armv7a)
 *
 * Copyright 2017, 2018 Phoenix Systems
 * Author; Pawel Pisarczyk, Aleksander Kaminski
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
.type sym, %function; \
sym: \
	.cfi_startproc; \
	push {r0-r3}; \
	.cfi_adjust_cfa_offset 16; \
	.cfi_rel_offset r0, 0; \
	.cfi_rel_offset r1, 4; \
	.cfi_rel_offset r2, 8; \
	.cfi_rel_offset r3, 12; \
	svc $sn; \
	add sp, #16; \
	.cfi_adjust_cfa_offset -16; \
	bx lr; \
	.cfi_endproc; \
.size sym, .-sym


.globl vfork;
.type vfork, %function;
vfork:
	b vforksvc
.size vfork, .-vfork


#define SYSCALLS_LIBC(name) \
	SYSCALLDEF(name, __COUNTER__);


SYSCALLS(SYSCALLS_LIBC)
