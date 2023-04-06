/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * syscalls (sparcv8leon3)
 *
 * Copyright 2022 Phoenix Systems
 * Author: Lukasz Leczkowski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#define __ASSEMBLY__
#include <phoenix/syscalls.h>

.section ".text"
.align 4


#define SYSCALLDEF(sym, sn) \
.globl sym; \
.type sym, #function; \
.align 4; \
sym: \
	save %sp, -0x60, %sp; \
	mov %i0, %o0; \
	mov %i1, %o1; \
	mov %i2, %o2; \
	mov %i3, %o3; \
	mov %i4, %o4; \
	mov %i5, %o5; \
	mov sn, %g4; \
	ta 0; \
	/* return value in %o0 */; \
	ret; \
	restore %g0, %o0, %o0; \
.size sym, .-sym


.globl vfork
.type vfork, #function
vfork:
	ba vforksvc
	nop
.size vfork, .-vfork


#define SYSCALLS_LIBC(name) \
	SYSCALLDEF(name, __COUNTER__);


SYSCALLS(SYSCALLS_LIBC)
