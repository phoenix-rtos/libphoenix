/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * setjmp, longjmp
 *
 * Copyright 2018-2019, 2023 Phoenix Systems
 * Author: Jan Sikorski, Andrzej Glowinski, Hubert Badocha
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

 /* jmp_buf layout:
 * esi
 * ebx
 * edi
 * ebp
 * esp + 4
 * return address
 * signal mask indicator (0 - not saved, otherwise saved(GOT addr on PIC))
 * signal mask (if saved)
 */

#define __ASSEMBLY__


.text


.macro __SETJMP
	/* Store registers into jmpbuf */
	leal 4(%esp),    %ecx
	movl  (%ecx),    %edx
	movl   %esi,    (%edx)
	movl   %ebx,   4(%edx)
	movl   %edi,   8(%edx)
	movl   %ebp,  12(%edx)
	/* Store stack pointer pointing before return address */
	movl   %ecx,  16(%edx)
	/* Store return address */
	movl  (%esp),    %ecx
	movl   %ecx,  20(%edx)
.endm


.globl _setjmp
.type _setjmp, %function
_setjmp:
	__SETJMP
	/* Set no mask indicator */
	movl   $0,    24(%edx)
	/* Store result (0) */
	xorl   %eax,     %eax
	ret
.size _setjmp, .-_setjmp


.globl setjmp
.type setjmp, %function
setjmp:
	__SETJMP
    /* Get signal mask */
#if __pic__ == 0
	pushl    $0
	pushl    $0
	call     signalMask
	addl     $8,      %esp
	/* Store signal mask indicator */
	movl     $1,   24(%edx)
#else
	pushl    %ebx

	call     .l1
.l1:
	popl     %ebx
	addl     $_GLOBAL_OFFSET_TABLE_+(.-.l1), %ebx

	pushl    $0
	pushl    $0
	call     signalMask@got(%ebx)
	addl     $8,      %esp
	/* Save GOT address as an indicator as it will not be 0 */
	movl     %ebx, 24(%edx)

	popl     %ebx
#endif
	/* Store signal mask */
	movl     %eax, 28(%edx)
	/* Store result (0) */
	xorl     $0,      %eax
	ret
.size setjmp, .-setjmp


.globl _longjmp
.type _longjmp, %function
_longjmp:
	/* Since esi will be restored later it can be used as a scratch register */
	/* Using it allows to safely call signalMask */
	movl   4(%esp), %esi
	/* Check mask indicator */
	/* Since ebx will be restored later it can be used as a scratch register */
	/* ebx as the mask indicator is a GOT address in PIC */
	movl  24(%esi), %ebx
	test     %ebx,  %ebx
	jz       1f
	/* Restore signal mask */
	pushl    $0xffffffff
	pushl    28(%esi)

#if __pic__ == 0
	call     signalMask
#else
	call     signalMask@got(%ebx)
#endif

	addl     $8,    %esp
1:
	/* Get second argument */
	movl   8(%esp), %eax
	/* _longjmp shall return 1 if 0 is provided as the second argument */
	movl     $1,    %ecx
	test     %eax,  %eax
	cmove    %ecx,  %eax
	/* Restore registers from jmpbuf */
	movl   4(%esi), %ebx
	movl   8(%esi), %edi
	movl  12(%esi), %ebp
	movl  16(%esi), %esp
	/* Jump point. Push return address to stack */
	pushl 20(%esi)
	/* At last restore esi */
	movl    (%esi), %esi
	ret
.size _longjmp, .-_longjmp


.globl sigsetjmp
.type sigsetjmp, %function
sigsetjmp:
	movl 8(%esp), %edx
	/* Call proper setjmp */
	test   %edx,  %edx
	jz     _setjmp
	jmp    setjmp
.size sigsetjmp, .-sigsetjmp
