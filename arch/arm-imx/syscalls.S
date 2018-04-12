/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * syscalls (arm-imx)
 *
 * Copyright 2017, 2018 Phoenix Systems
 * Author; Pawel Pisarczyk, Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#define __ASSEMBLY__
#include "../../../phoenix-rtos-kernel/include/syscalls.h"

.text


#define SYSCALLDEF(sym, sn) \
.globl sym; \
.type sym, %function; \
sym: \
	push {r0-r3}; \
	svc $sn; \
	add sp, #16; \
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
