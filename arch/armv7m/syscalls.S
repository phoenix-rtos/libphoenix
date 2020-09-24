/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * syscalls (armv7)
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

.text


#define SYSCALLDEF(sym, sn) \
.globl sym; \
.type sym, %function; \
sym: \
	svc $sn; \
	bx lr; \
.size sym, .-sym


.globl vfork;
.type vfork, %function;
vfork:
	b vforksvc
.size vfork, .-vfork


#define SYSCALLS_LIBC(name) \
	SYSCALLDEF(name, __COUNTER__);


SYSCALLS(SYSCALLS_LIBC)
