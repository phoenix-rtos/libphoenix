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

/* TODO */
.globl _setjmp
.type _setjmp, #function
_setjmp:
	retl
	mov -ENOSYS, %o0
.size _setjmp, .-_setjmp


.globl setjmp
.type setjmp, #function
setjmp:
	retl
	mov -ENOSYS, %o0
.size setjmp, .-setjmp


.globl _longjmp
.type _longjmp, #function
_longjmp:
	retl
	mov -ENOSYS, %o0
.size _longjmp, .-_longjmp


.globl sigsetjmp
.type sigsetjmp, #function
sigsetjmp:
	retl
	mov -ENOSYS, %o0
.size sigsetjmp, .-sigsetjmp
