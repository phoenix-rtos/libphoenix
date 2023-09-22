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
	mov sn, %g4; \
	ta 0; \
	/* return value in %o0 */; \
	nop; /* TN-0018 fix */ \
	retl; \
	nop; \
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
