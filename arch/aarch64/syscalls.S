/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * syscalls (aarch64)
 *
 * Copyright 2024 Phoenix Systems
 * Author: Pawel Pisarczyk, Aleksander Kaminski, Jacek Maksymowicz
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
	stp x0, x1, [sp, #-0x40]!; \
	stp x2, x3, [sp, #0x10]; \
	stp x4, x5, [sp, #0x20]; \
	stp x6, x7, [sp, #0x30]; \
	.cfi_adjust_cfa_offset 64; \
	.cfi_rel_offset x0, 0; \
	.cfi_rel_offset x1, 8; \
	.cfi_rel_offset x2, 16; \
	.cfi_rel_offset x3, 24; \
	.cfi_rel_offset x4, 32; \
	.cfi_rel_offset x5, 40; \
	.cfi_rel_offset x6, 48; \
	.cfi_rel_offset x7, 56; \
	svc sn; \
	add sp, sp, #0x40; \
	.cfi_adjust_cfa_offset -64; \
	ret; \
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
