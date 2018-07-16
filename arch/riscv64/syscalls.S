/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * syscalls (RISCV64)
 *
 * Copyright 2018 Phoenix Systems
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
sym: \
	li a0, sn; \
	scall; \
	ret; \
.size sym, .-sym


.globl vfork;
.type vfork, %function;
vfork:
	j vforksvc
.size vfork, .-vfork


#define SYSCALLS_LIBC(name) \
	SYSCALLDEF(name, __COUNTER__);


SYSCALLS(SYSCALLS_LIBC)
