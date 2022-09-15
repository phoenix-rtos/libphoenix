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
	st  %i7, [%sp + 0x3c]; \
	mov %o7, %i7; \
	sub %sp, 0x60, %sp; \
	mov sn, %g4; \
	ta 0; \
	mov %i7, %o7; \
	ld [%sp + 0x9c], %i7; \
	retl; \
	add %sp, 0x60, %sp; \
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
