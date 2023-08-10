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
	.cfi_startproc; \
	addi sp, sp, -64; \
	sd   a0,   (sp); \
	sd   a1,  8(sp); \
	sd   a2, 16(sp); \
	sd   a3, 24(sp); \
	sd   a4, 32(sp); \
	sd   a5, 40(sp); \
	sd   a6, 48(sp); \
	sd   a7, 56(sp); \
	.cfi_adjust_cfa_offset 64; \
	.cfi_rel_offset a0, 0; \
	.cfi_rel_offset a1, 8; \
	.cfi_rel_offset a2, 16; \
	.cfi_rel_offset a3, 24; \
	.cfi_rel_offset a4, 32; \
	.cfi_rel_offset a5, 40; \
	.cfi_rel_offset a6, 48; \
	.cfi_rel_offset a7, 56; \
	li a7, sn; \
	ecall; \
	add sp, sp, 64; \
	.cfi_adjust_cfa_offset -64; \
	ret; \
	.cfi_endproc; \
.size sym, .-sym


.globl vfork;
.type vfork, %function;
vfork:
	j vforksvc
.size vfork, .-vfork


#define SYSCALLS_LIBC(name) \
	SYSCALLDEF(name, __COUNTER__);


SYSCALLS(SYSCALLS_LIBC)
