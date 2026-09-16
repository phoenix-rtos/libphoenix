/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * syscalls (armv7m)
 *
 * Copyright 2017 Phoenix Systems
 * Author; Pawel Pisarczyk, Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#define __ASSEMBLY__
#include <phoenix/syscalls.h>


#define SYSCALLDEF(sym, sn) \
.section .text.sym, "ax", %progbits; \
.globl sym; \
.type sym, %function; \
.balign 4; \
sym: \
	svc $sn; \
	bx lr; \
.size sym, .-sym


.section .text.vfork, "ax", %progbits;
.globl vfork;
.type vfork, %function;
.balign 4;
vfork:
	b vforksvc
.size vfork, .-vfork


#define SYSCALLS_LIBC(name) \
	SYSCALLDEF(name, __COUNTER__);


SYSCALLS(SYSCALLS_LIBC)
