/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * setjmp, longjmp (RISCV64)
 *
 * Copyright 2020 Phoenix Systems
 * Author: Paweł Pisarczyk
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#define __ASSEMBLY__


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

j _setjmp

	/* Store stack pointer */
	sd sp,  96(a0)

	/* Store jump address */
	la t0, (1f)
	sd t0, 102(a0)

	/* Store return address */
	sd ra, 110(a0)

	/* Set result */
	mv a0, zero
	ret
1:
	ld ra, 110(a0)
	mv a0, a3 /* longjump puts argument in a3 */
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
2:
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
	j 2b

	/* Store stack pointer */
	sd sp,  96(a0)

	/* Store jump address */
	la t0, (1f)
	sd t0, 102(a0)

	/* Store return address */
	sd ra, 110(a0)

	/* Store signal mask */
	sd a3, 118(a0)

	/* Set result */
	mv a0, zero
	ret
1:
	/* this is where we land after the jump
	 * we want to restore signal mask */

	addi sp, sp, -24
	sd   a0,   (sp)
	sd   s0,  8(sp)
	sd   ra, 16(sp)

	ld   s0, 118(a0)
	mv   s0, a3
	li   a1, -1
	call signalMask
	mv   a0, s0

	ld   a0,   (sp)
	ld   s0,  8(sp)
	ld   ra, 16(sp)
	addi sp, sp, 24
	jr   ra

.size setjmp, .-setjmp


.globl _longjmp
.type _longjmp, %function
_longjmp:
	/* move return value to r3.
	 * longjmp should not make setjmp return 0 though */
	mv     a3, a0
	bnez   a3, 1f
	addi   a3, a3, 1
1:
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
	j 1b

	/* Restore stack pointer */
	ld sp,  96(a0)

	/* Retore jump address */
	ld t0, 102(a0)

	/* Retore return address */
	sd ra, 110(a0)

	jr t0

.size _longjmp, .-_longjmp


.globl sigsetjmp
.type sigsetjmp, %function
sigsetjmp:
	bne a1, zero, setjmp
	j _setjmp
.size sigsetjmp, .-sigsetjmp
