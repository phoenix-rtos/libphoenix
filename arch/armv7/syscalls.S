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
#include "../../../phoenix-rtos-kernel/include/syscall.h"

.text


#define SYSCALLDEF(sym, sn) \
.globl sym; \
.type sym, %function; \
sym: \
	svc $sn; \
	blx lr; \
.size sym, .-sym


.globl vfork;
.type vfork, %function;
vfork:
	b vforksvc
.size vfork, .-vfork


#define SYSCALLS_LIBC(n, kernel, libc) \
	SYSCALLDEF(libc, n);


SYSCALLS(SYSCALLS_LIBC)
