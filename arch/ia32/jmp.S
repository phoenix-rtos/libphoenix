/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * setjmp, longjmp
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#define __ASSEMBLY__


.text

.globl setjmp
.type setjmp, %function
setjmp:
	mov 4(%esp),   %eax
	mov   %ebx,   (%eax)
	mov   %edi,  8(%eax)
	mov   %ebp, 12(%eax)
	lea 4(%esp),   %ecx
	mov   %ecx, 16(%eax)
	mov  (%esp),   %ecx
	mov   %ecx, 20(%eax)
	mov    $0,     %eax
	ret
.size setjmp, .-setjmp


.globl longjmp
.type longjmp, %function
longjmp:
	mov  4(%esp), %edx
	mov  8(%esp), %eax
	test   %eax,  %eax
	jnz 1f
	inc    %eax
1:	mov   (%edx), %ebx
	mov  4(%edx), %esi
	mov  8(%edx), %edi
	mov 12(%edx), %ebp
	mov 16(%edx), %esp
	mov 20(%edx), %ecx
	jmp *%ecx
.size longjmp, .-longjmp
