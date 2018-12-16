/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 *
 * _setjmp, _longjmp, setjmp, sigsetjmp
 *
 * Copyright 2018 Phoenix Systems
 * Author: Kamil Amanowicz
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#define __ASSEMBLY__

.thumb
.syntax unified

.globl _setjmp
.type _setjmp, %function
_setjmp:
	/* set jump address and stack pointer */
	ldr r1, =(1f + 1)
	mov r2, sp
	/* store everythig in jmpbuf */
	stm r0, {r1, r2, r4, r5, r6, r7, r8, r10, r11, lr}
	mov r0, #0
	b 2f
1:
	/* this is where we land after the jump
	 * just pass the return value which is in r3 */
	mov r0, r3
2:
	bx lr
.size _setjmp, .-_setjmp

.globl _longjmp
.type _longjmp, %function
_longjmp:
	/* move return value to r3 */
	mov r3, r1
	/* restore registers from jmpbuf */
	ldmia r0!, {r1, r2, r4, r5, r6, r7, r8, r10, r11, lr}
	mov sp, r2
	bx r1
.size _longjmp, .-_longjmp

.globl setjmp
.type setjmp, %function
setjmp:
	push {r0, lr}
	/* get signal mask */
	mov r0, #0
	mov r1, #0
	bl signalMask
	mov r3, r0
	pop {r0, lr}
	/* store everything in jmpbuf */
	ldr r1, =(1f + 1)
	mov r2, sp
	stmia r0!, {r1, r2, r4, r5, r6, r7, r8, r10, r11, lr}
	str r3, [r0]
	mov r0, #0
	b 2f
1:
	/* this is where we land after the jump
	 * r0 is now pointing to signal mask
	 * which we want to restore */
	push {r1, r4, lr}
	ldr r4, [r0]
	mov r0, r4
	mov r4, r3
	ldr r1, =0xffffffff
	bl signalMask
	/* set return value and pop the stack */
	mov r0, r4
	pop {r1, r4, lr}
2:
	bx lr
.size setjmp, .-setjmp

.globl sigsetjmp
.type sigsetjmp, %function
sigsetjmp:
	cmp r1, #0
	bne setjmp
	b _setjmp
.size sigsetjmp, .-sigsetjmp
