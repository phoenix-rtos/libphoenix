/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * setjmp, longjmp (sparcv8leon3)
 *
 * Copyright 2022 Phoenix Systems
 * Author: Lukasz Leczkowski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */


#define __ASSEMBLY__

#include <phoenix/errno.h>

.section ".text"
.align 4

/* jmp_buf layout:
 * Caller's stack pointer
 * (Return address - 8)
 * Frame pointer and caller's return address
 * Signal mask indicator (0 - not saved, 1 - saved)
 * Signal mask (if saved)
 */

.globl _setjmp
.type _setjmp, #function
_setjmp:
	st  %sp, [%o0]
	st  %o7, [%o0 + 0x4]
	std %i6, [%o0 + 0x8]
	st  %g0, [%o0 + 0x10]
	retl
	clr %o0
.size _setjmp, .-_setjmp


.globl setjmp
.type setjmp, #function
setjmp:
	/* Save registers without changing windows yet */
	st  %sp, [%o0]
	st  %o7, [%o0 + 0x4]
	std %i6, [%o0 + 0x8]

	save %sp, -0x60, %sp

	/* get signal mask */
	mov %g0, %o0
	call signalMask
	mov %g0, %o1

	st  %o0, [%i0 + 0x14] /* signal mask */
	mov 1, %o0
	st  %o0, [%i0 + 0x10] /* signal mask indicator */

	ret
	restore %g0, 0, %o0
.size setjmp, .-setjmp


.globl _longjmp
.type _longjmp, #function
_longjmp:
	ta 0x3 /* flush windows */
	/* calculate return value */
	cmp %o1, 0
	bne 1f
	mov %o1, %g1
	mov 1, %g1
1:
	/* restore registers */
	ld  [%o0], %sp
	ld  [%o0 + 0x4], %o7
	ldd [%o0 + 0x8], %i6
	ldd [%sp], %l0
	ldd [%sp + 0x08], %l2
	ldd [%sp + 0x10], %l4
	ldd [%sp + 0x18], %l6
	ldd [%sp + 0x20], %i0
	ldd [%sp + 0x28], %i2
	ldd [%sp + 0x30], %i4

	/* Check if signal mask was saved */
	ld  [%o0 + 0x10], %o1
	cmp %o1, 0
	be 2f
	nop

	save %sp, -0x60, %sp
	mov %g1, %l0
	/* restore signal mask */
	set 0xffffffff, %o1
	call signalMask
	ld  [%i0 + 0x14], %o0
	mov %l0, %g1
	ret
	restore %g1, 0, %o0
2:
	retl
	mov %g1, %o0
.size _longjmp, .-_longjmp


.globl sigsetjmp
.type sigsetjmp, #function
sigsetjmp:
	cmp %o1, 0
	bne setjmp
	nop
	ba _setjmp
	nop
.size sigsetjmp, .-sigsetjmp
