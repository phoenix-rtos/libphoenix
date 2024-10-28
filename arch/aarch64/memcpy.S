/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * memcpy
 * Adapted from work by Arm Limited, licensed under MIT license.
 *
 * Copyright 2024 Phoenix Systems
 * Author: Jacek Maksymowicz
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

/*
 * memcpy - copy memory area
 *
 * Copyright (c) 2012-2022, Arm Limited.
 * SPDX-License-Identifier: MIT OR Apache-2.0 WITH LLVM-exception
 */

/* Assumptions:
 *
 * ARMv8-a, AArch64, unaligned accesses.
 *
 */

#define DSTIN  x0
#define SRC    x1
#define COUNT  x2
#define DST    x3
#define SRCEND x4
#define DSTEND x5
#define A_L    x6
#define A_LW   w6
#define A_H    x7
#define B_L    x8
#define B_LW   w8
#define B_H    x9
#define C_L    x10
#define C_LW   w10
#define C_H    x11
#define D_L    x12
#define D_H    x13
#define E_L    x14
#define E_H    x15
#define F_L    x16
#define F_H    x17
#define G_L    COUNT
#define G_H    DST
#define H_L    SRC
#define H_H    SRCEND
#define tmp1   x14

/* This implementation handles overlaps and supports both memcpy and memmove
   from a single entry point.  It uses unaligned accesses and branchless
   sequences to keep the code small, simple and improve performance.

   Copies are split into 3 main cases: small copies of up to 32 bytes, medium
   copies of up to 128 bytes, and large copies.  The overhead of the overlap
   check is negligible since it is only required for large copies.

   Large copies use a software pipelined loop processing 64 bytes per iteration.
   The destination pointer is 16-byte aligned to minimize unaligned accesses.
   The loop tail is handled by always copying 64 bytes from the end.
*/

.globl memmove
.type memmove, %function
memmove:
.globl memcpy
.type memcpy, %function
memcpy:
.cfi_startproc
	add SRCEND, SRC, COUNT
	add DSTEND, DSTIN, COUNT
	cmp COUNT, 128
	b.hi .Lcopy_long
	cmp COUNT, 32
	b.hi .Lcopy32_128

	/* Small copies: 0..32 bytes.  */
	cmp COUNT, 16
	b.lo .Lcopy16
	ldp A_L, A_H, [SRC]
	ldp D_L, D_H, [SRCEND, -16]
	stp A_L, A_H, [DSTIN]
	stp D_L, D_H, [DSTEND, -16]
	ret

	/* Copy 8-15 bytes.  */
.Lcopy16:
	tbz COUNT, 3, .Lcopy8
	ldr A_L, [SRC]
	ldr A_H, [SRCEND, -8]
	str A_L, [DSTIN]
	str A_H, [DSTEND, -8]
	ret

	.p2align 3
	/* Copy 4-7 bytes.  */
.Lcopy8:
	tbz COUNT, 2, .Lcopy4
	ldr A_LW, [SRC]
	ldr B_LW, [SRCEND, -4]
	str A_LW, [DSTIN]
	str B_LW, [DSTEND, -4]
	ret

	/* Copy 0..3 bytes using a branchless sequence.  */
.Lcopy4:
	cbz COUNT, .Lcopy0
	lsr tmp1, COUNT, 1
	ldrb A_LW, [SRC]
	ldrb C_LW, [SRCEND, -1]
	ldrb B_LW, [SRC, tmp1]
	strb A_LW, [DSTIN]
	strb B_LW, [DSTIN, tmp1]
	strb C_LW, [DSTEND, -1]
.Lcopy0:
	ret

	.p2align 4
	/* Medium copies: 33..128 bytes.  */
.Lcopy32_128:
	ldp A_L, A_H, [SRC]
	ldp B_L, B_H, [SRC, 16]
	ldp C_L, C_H, [SRCEND, -32]
	ldp D_L, D_H, [SRCEND, -16]
	cmp COUNT, 64
	b.hi .Lcopy128
	stp A_L, A_H, [DSTIN]
	stp B_L, B_H, [DSTIN, 16]
	stp C_L, C_H, [DSTEND, -32]
	stp D_L, D_H, [DSTEND, -16]
	ret

	.p2align 4
	/* Copy 65..128 bytes.  */
.Lcopy128:
	ldp E_L, E_H, [SRC, 32]
	ldp F_L, F_H, [SRC, 48]
	cmp COUNT, 96
	b.ls .Lcopy96
	ldp G_L, G_H, [SRCEND, -64]
	ldp H_L, H_H, [SRCEND, -48]
	stp G_L, G_H, [DSTEND, -64]
	stp H_L, H_H, [DSTEND, -48]
.Lcopy96:
	stp A_L, A_H, [DSTIN]
	stp B_L, B_H, [DSTIN, 16]
	stp E_L, E_H, [DSTIN, 32]
	stp F_L, F_H, [DSTIN, 48]
	stp C_L, C_H, [DSTEND, -32]
	stp D_L, D_H, [DSTEND, -16]
	ret

	.p2align 4
	/* Copy more than 128 bytes.  */
.Lcopy_long:
	/* Use backwards copy if there is an overlap.  */
	sub tmp1, DSTIN, SRC
	cbz tmp1, .Lcopy0
	cmp tmp1, COUNT
	b.lo .Lcopy_long_backwards

	/* Copy 16 bytes and then align DST to 16-byte alignment.  */

	ldp D_L, D_H, [SRC]
	and tmp1, DSTIN, 15
	bic DST, DSTIN, 15
	sub SRC, SRC, tmp1
	add COUNT, COUNT, tmp1  /* Count is now 16 too large.  */
	ldp A_L, A_H, [SRC, 16]
	stp D_L, D_H, [DSTIN]
	ldp B_L, B_H, [SRC, 32]
	ldp C_L, C_H, [SRC, 48]
	ldp D_L, D_H, [SRC, 64]!
	subs COUNT, COUNT, 128 + 16  /* Test and readjust COUNT.  */
	b.ls .Lcopy64_from_end

.Lloop64:
	stp A_L, A_H, [DST, 16]
	ldp A_L, A_H, [SRC, 16]
	stp B_L, B_H, [DST, 32]
	ldp B_L, B_H, [SRC, 32]
	stp C_L, C_H, [DST, 48]
	ldp C_L, C_H, [SRC, 48]
	stp D_L, D_H, [DST, 64]!
	ldp D_L, D_H, [SRC, 64]!
	subs COUNT, COUNT, 64
	b.hi .Lloop64

	/* Write the last iteration and copy 64 bytes from the end.  */
.Lcopy64_from_end:
	ldp E_L, E_H, [SRCEND, -64]
	stp A_L, A_H, [DST, 16]
	ldp A_L, A_H, [SRCEND, -48]
	stp B_L, B_H, [DST, 32]
	ldp B_L, B_H, [SRCEND, -32]
	stp C_L, C_H, [DST, 48]
	ldp C_L, C_H, [SRCEND, -16]
	stp D_L, D_H, [DST, 64]
	stp E_L, E_H, [DSTEND, -64]
	stp A_L, A_H, [DSTEND, -48]
	stp B_L, B_H, [DSTEND, -32]
	stp C_L, C_H, [DSTEND, -16]
	ret

	.p2align 4

	/* Large backwards copy for overlapping copies.
	   Copy 16 bytes and then align DST to 16-byte alignment.  */
.Lcopy_long_backwards:
	ldp D_L, D_H, [SRCEND, -16]
	and tmp1, DSTEND, 15
	sub SRCEND, SRCEND, tmp1
	sub COUNT, COUNT, tmp1
	ldp A_L, A_H, [SRCEND, -16]
	stp D_L, D_H, [DSTEND, -16]
	ldp B_L, B_H, [SRCEND, -32]
	ldp C_L, C_H, [SRCEND, -48]
	ldp D_L, D_H, [SRCEND, -64]!
	sub DSTEND, DSTEND, tmp1
	subs COUNT, COUNT, 128
	b.ls .Lcopy64_from_start

.Lloop64_backwards:
	stp A_L, A_H, [DSTEND, -16]
	ldp A_L, A_H, [SRCEND, -16]
	stp B_L, B_H, [DSTEND, -32]
	ldp B_L, B_H, [SRCEND, -32]
	stp C_L, C_H, [DSTEND, -48]
	ldp C_L, C_H, [SRCEND, -48]
	stp D_L, D_H, [DSTEND, -64]!
	ldp D_L, D_H, [SRCEND, -64]!
	subs COUNT, COUNT, 64
	b.hi .Lloop64_backwards

	/* Write the last iteration and copy 64 bytes from the start.  */
.Lcopy64_from_start:
	ldp G_L, G_H, [SRC, 48]
	stp A_L, A_H, [DSTEND, -16]
	ldp A_L, A_H, [SRC, 32]
	stp B_L, B_H, [DSTEND, -32]
	ldp B_L, B_H, [SRC, 16]
	stp C_L, C_H, [DSTEND, -48]
	ldp C_L, C_H, [SRC]
	stp D_L, D_H, [DSTEND, -64]
	stp G_L, G_H, [DSTIN, 48]
	stp A_L, A_H, [DSTIN, 32]
	stp B_L, B_H, [DSTIN, 16]
	stp C_L, C_H, [DSTIN]
	ret

.cfi_endproc
.size memcpy, .-memcpy
.size memmove, .-memmove
.ltorg
