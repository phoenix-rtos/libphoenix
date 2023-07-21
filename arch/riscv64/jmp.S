/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * setjmp, longjmp (RISCV64)
 *
 * Copyright 2020, 2023 Phoenix Systems
 * Author: Pawel Pisarczyk, Lukasz Leczkowski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#define __ASSEMBLY__

/* jmp_buf layout:
 * s0 - s11: saved registers
 * sp: stack pointer
 * ra: return address
 * signal mask indicator (0 - not saved, 1 - saved)
 * signal mask (if saved)
 */

.text

.globl _setjmp
.type _setjmp, %function
_setjmp:
	/* Store registers into jmpbuf */
	sd s0,    (a0)
	sd s1,   8(a0)
	sd s2,  16(a0)
	sd s3,  24(a0)
	sd s4,  32(a0)
	sd s5,  40(a0)
	sd s6,  48(a0)
	sd s7,  56(a0)
	sd s8,  64(a0)
	sd s9,  72(a0)
	sd s10, 80(a0)
	sd s11, 88(a0)

	sd sp,  96(a0)

	/* return address */
	sd ra, 104(a0)

	/* signal mask indicator */
	sd zero, 112(a0)

	/* return value */
	mv a0, zero
	ret
.size _setjmp, .-_setjmp


.globl setjmp
.type setjmp, %function
setjmp:
	addi sp, sp, -16
	sd   a0,  (sp)
	sd   ra, 8(sp)

	mv   a0, zero
	mv   a1, zero
	call signalMask
	mv   a3, a0
	ld   a0,  (sp)
	ld   ra, 8(sp)
	addi sp, sp, 16

	/* Store registers into jmpbuf */
	sd s0,    (a0)
	sd s1,   8(a0)
	sd s2,  16(a0)
	sd s3,  24(a0)
	sd s4,  32(a0)
	sd s5,  40(a0)
	sd s6,  48(a0)
	sd s7,  56(a0)
	sd s8,  64(a0)
	sd s9,  72(a0)
	sd s10, 80(a0)
	sd s11, 88(a0)

	sd sp,  96(a0)

	/* return address */
	sd ra, 104(a0)

	/* signal mask indicator */
	li a1, 1
	sd a1, 112(a0)

	/* signal mask */
	sd a3, 120(a0)

	/* return value */
	mv a0, zero
	ret
.size setjmp, .-setjmp


.globl _longjmp
.type _longjmp, %function
_longjmp:
	/* calculate return value */
	mv s1, a1
	bnez s1, 1f
	li s1, 1
1:
	/* check if signal mask was saved */
	mv s0, a0
	ld a1, 112(a0)
	beqz a1, 2f

	/* restore signal mask */
	ld a0, 120(a0)
	li a1, -1
	call signalMask
	mv a0, s0

2:
	mv a1, s1

	/* restore registers from jmpbuf */
	ld s0,    (a0)
	ld s1,   8(a0)
	ld s2,  16(a0)
	ld s3,  24(a0)
	ld s4,  32(a0)
	ld s5,  40(a0)
	ld s6,  48(a0)
	ld s7,  56(a0)
	ld s8,  64(a0)
	ld s9,  72(a0)
	ld s10, 80(a0)
	ld s11, 88(a0)

	ld sp,  96(a0)

	/* return address */
	ld ra, 104(a0)

	/* return value */
	mv a0, a1
	ret
.size _longjmp, .-_longjmp


.globl sigsetjmp
.type sigsetjmp, %function
sigsetjmp:
	bne a1, zero, setjmp
	j _setjmp
.size sigsetjmp, .-sigsetjmp
