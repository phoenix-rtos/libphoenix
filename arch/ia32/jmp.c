/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * setjmp/longjmp
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <errno.h>
#include <setjmp.h>


/* UNTESTED */
int setjmp(jmp_buf var)
{
	__asm__ volatile (
		"mov 4(%esp),   %eax  ;"
		"mov   %ebx,   (%eax) ;"
		"mov   %esi,  4(%eax) ;"
		"mov   %edi,  8(%eax) ;"
		"mov   %ebp, 12(%eax) ;"
		"lea 8(%esp),   %ebx  ;"
		"mov   %ebx, 16(%eax) ;"
		"mov  (%esp),   %ecx  ;"
		"mov   %ecx, 20(%eax) ;"
		"xor   %eax,    %eax  ;");

    return 0;
}


/* UNTESTED */
void longjmp(jmp_buf var, int m)
{
	__asm__ volatile (
		"mov  4(%esp), %edx ;"
		"mov  8(%esp), %eax ;"
		"test   %eax,  %eax ;"
		"jnz 1f             ;"
		"inc    %eax        ;"
		"1: "
		"mov   (%edx), %ebx ;"
		"mov  4(%edx), %esi ;"
		"mov  8(%edx), %edi ;"
		"mov 12(%edx), %ebp ;"
		"mov 16(%edx), %ecx ;"
		"mov    %ecx,  %esp ;"
		"mov 20(%edx), %ecx ;"
		"jmp   *%ecx");
}
