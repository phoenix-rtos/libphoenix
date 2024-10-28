/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * _setjmp, _longjmp, setjmp, sigsetjmp
 *
 * Copyright 2018, 2019, 2023, 2024 Phoenix Systems
 * Author: Kamil Amanowicz, Aleksander Kaminski, Jakub Sarzynski, Jacek Maksymowicz
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

/* setjmp env buffer description
 * Offset  Description
 * 0x00    Callee-saved registers (x19-x30), includes FP and LR
 * 0x60    SP register
 * 0x68    Signal mask flag - if != 0 then next value stores a signal mask
 * 0x6c    Signal mask (or dummy value if signal mask flag == 0)
 * 0x70    FPCR
 * 0x78    FPSR
 * 0x80    Callee-saved FPU/SIMD registers (d8-d15)
 */

#define __ASSEMBLY__

.text

.globl sigsetjmp
.type sigsetjmp, %function
sigsetjmp:
	stp x19, x20, [x0, #0x00]
	stp x21, x22, [x0, #0x10]
	stp x23, x24, [x0, #0x20]
	stp x25, x26, [x0, #0x30]
	stp x27, x28, [x0, #0x40]
	stp x29, x30, [x0, #0x50]

	mov x2, sp
	str x2, [x0, #0x60]

	/* Store signal mask if savesigs != 0 */
	mov w2, #0
	cbz w1, 1f

	/* Get signal mask */
	stp x0, x1, [sp, -0x10]! /* Store function args */
	mov w0, #0
	movn w1, #0
	bl signalMask
	mov w2, w0
	ldp x0, x1, [sp], #0x10 /* Restore function args */
	ldr x30, [x0, #0x58] /* Restore LR */

1:
	/* Store flag and mask (fake or real) */
	stp w1, w2, [x0, #0x68]

#ifndef __SOFTFP__
	mrs x1, fpcr
	mrs x2, fpsr
	stp x1, x2, [x0, #0x70]

	/* Store callee-saved fpu registers */
	stp  d8,  d9, [x0, #0x80]
	stp d10, d11, [x0, #0x90]
	stp d12, d13, [x0, #0xa0]
	stp d14, d15, [x0, #0xb0]
#endif

	mov x0, #0
	ret
.size sigsetjmp, .-sigsetjmp


.globl _setjmp
.type _setjmp, %function
_setjmp:
	mov x1, #0
	b sigsetjmp
.size _setjmp, .-_setjmp


.globl setjmp
.type setjmp, %function
setjmp:
	mov x1, #1
	b sigsetjmp
.size setjmp, .-setjmp


.globl _longjmp
.type _longjmp, %function
_longjmp:
#ifndef __SOFTFP__
	ldp x2, x3, [x0, #0x70]
	msr fpcr, x2
	msr fpsr, x3

	ldp  d8,  d9, [x0, #0x80]
	ldp d10, d11, [x0, #0x90]
	ldp d12, d13, [x0, #0xa0]
	ldp d14, d15, [x0, #0xb0]
#endif

	/* Load flag and mask */
	ldp w2, w3, [x0, #0x68]
	cbz w2, 1f

	/* Restore mask */
	stp x0, x1, [sp, -0x10]! /* Store function args */
	mov w0, w3
	movn w1, #0
	bl signalMask
	ldp x0, x1, [sp], #0x10 /* Restore function args */

1:
	/* Restore general registers */
	ldp x19, x20, [x0, #0x00]
	ldp x21, x22, [x0, #0x10]
	ldp x23, x24, [x0, #0x20]
	ldp x25, x26, [x0, #0x30]
	ldp x27, x28, [x0, #0x40]
	ldp x29, x30, [x0, #0x50]

	ldr x2, [x0, #0x60]
	mov sp, x2

	/* Set return value */
	cmp  w1, #0
	csinc w0, w1, w1, ne
	ret
.size _longjmp, .-_longjmp
