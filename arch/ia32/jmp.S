/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * setjmp, longjmp
 *
 * Copyright 2018-2019 Phoenix Systems
 * Author: Jan Sikorski, Andrzej Glowinski
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
	movl 4(%esp),   %edx
	movl   %ebx,   (%edx)
	movl   %esi,  4(%edx)
	movl   %edi,  8(%edx)
	movl   %ebp, 12(%edx)
	/* Store stack pointer pointing before return address */
	leal 4(%esp),   %ecx
	movl   %ecx, 16(%edx)
	/* Store jump address */
	leal  (1f),     %ecx
	movl   %ecx, 20(%edx)
	/* Store return address */
	movl  (%esp),   %ecx
	movl  %ecx,  24(%edx)
	/* Store result */
	movl  $0,       %eax
	ret
1:
	/* Jump point. Push return address to stack */
	pushl  24(%edx)
	ret
.size _setjmp, .-_setjmp


.globl setjmp
.type setjmp, %function
setjmp:
        /* Get signal mask */
	pushl  $0
	pushl  $0
	call   signalMask
	addl   $8,      %esp
	/* Store registers into jmpbuf */
	movl 4(%esp),   %edx
	movl   %ebx,   (%edx)
	movl   %esi,  4(%edx)
	movl   %edi,  8(%edx)
	movl   %ebp, 12(%edx)
	/* Store stack pointer pointing before return address */
	leal 4(%esp),   %ecx
	movl   %ecx, 16(%edx)
	/* Store jump address */
	leal  (1f),     %ecx
	movl   %ecx, 20(%edx)
	/* Store return address */
	movl  (%esp),   %ecx
	movl   %ecx, 24(%edx)
	/* Store signal mask */
	movl   %eax, 28(%edx)
	/* Store result */
	movl   $0,      %eax
	ret
1:
	/* Jump point. Push return address to stack */
	pushl 24(%edx)
	/* Store longjmp return value */
	pushl    %eax
	/* Restore signal mask */
	movl  28(%edx), %ecx
	pushl    $0xffffffff
	pushl    %ecx
	call     signalMask
	addl     $8,    %esp
	/* Restore longjmp return value */
	popl     %eax
	ret
.size setjmp, .-setjmp


.globl _longjmp
.type _longjmp, %function
_longjmp:
	movl  4(%esp), %edx
	movl  8(%esp), %eax
	test   %eax,  %eax
	jnz    1f
	inc    %eax
1:
	/* Restore registers from jmpbuf */
	movl   (%edx), %ebx
	movl  4(%edx), %esi
	movl  8(%edx), %edi
	movl 12(%edx), %ebp
	movl 16(%edx), %esp
	/* Restore jump address */
	movl 20(%edx), %ecx
	/* Jump */
	jmp    *%ecx
.size _longjmp, .-_longjmp


.globl sigsetjmp
.type sigsetjmp, %function
sigsetjmp:
	/* Modify stack frame to skip this function when returning from setjmp */
	movl  8(%esp), %edx
	movl  4(%esp), %ecx
	movl  (%esp),  %eax
	addl   $8,     %esp
	pushl  %ecx
	pushl  %eax

	/* Call proper setjmp */
	test    %edx,  %edx
	jne setjmp
	jmp _setjmp
.size sigsetjmp, .-sigsetjmp
