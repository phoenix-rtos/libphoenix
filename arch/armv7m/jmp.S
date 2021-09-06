/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 *
 * _setjmp, _longjmp, setjmp, sigsetjmp
 *
 * Copyright 2018, 2019 Phoenix Systems
 * Author: Kamil Amanowicz, Aleksander Kaminski, Jakub Sarzyński
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

 /* setjmp env buffer description
  * Offset	Description
  * 0		Signal mask flag - if != 0 then next value stores a signal mask
  * 4		Signal mask
  * 8		SP register
  * 12-44	r4-r11 registers
  * 44		LR register
  * 48-80	d8-d15 registers
  * 80		fpscr register
  */

#define __ASSEMBLY__

.thumb
.syntax unified


.globl sigsetjmp
.type sigsetjmp, %function
sigsetjmp:
	/* Store signal mask if savesigs != 0 */
	mov r2, #0
	cbz r1, 1f

	/* Get signal mask */
	/* Stack must be aligned to 8 bytes */
	push {r0, r1, r3, lr}
	mov r0, #0
	mov r1, r0
	bl signalMask
	mov r2, r0
	pop {r0, r1, r3, lr}

1:
	/* Store mask (fake or real) and general registers */
	mov r3, sp
	stmia r0!, {r1-r11, lr}

#ifndef __SOFTFP__
	/* Store fpu registers */
	vstm r0!, {d8-d15}
	vmrs r2, fpscr
	str r2, [r0], #4
#endif

	mov r0, #0
	bx lr
.size sigsetjmp, .-sigsetjmp

.globl _setjmp
.type _setjmp, %function
_setjmp:
	mov r1, #0
	b sigsetjmp
.size _setjmp, .-_setjmp

.globl setjmp
.type setjmp, %function
setjmp:
	mov r1, #1
	b sigsetjmp
.size setjmp, .-setjmp


.globl _longjmp
.type _longjmp, %function
_longjmp:
	/* Check for signal mask in env buffer, if it's saved restore it */
	ldmia r0!, {r2-r3}
	cbz r2, 1f

	/* Restore mask */
	push {r0, r1}
	mov r0, r3
	mvn r1, #0
	bl signalMask
	pop {r0, r1}

1:
	/* Restore general registers */
	ldmia r0!, {r2, r4-r11, lr}
	mov sp, r2

#ifndef __SOFTFP__
	/* Restore fpu registers */
	vldm r0!, {d8-d15}
	ldr r2, [r0], #4
	vmsr fpscr, r2
#endif

	/* Set return value */
	movs r0, r1
	it eq
	moveq r0, #1
	bx lr
.size _longjmp, .-_longjmp


