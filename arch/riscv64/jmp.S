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
.size _setjmp, .-_setjmp


.globl setjmp
.type setjmp, %function
setjmp:
.size setjmp, .-setjmp


.globl _longjmp
.type _longjmp, %function
_longjmp:
.size _longjmp, .-_longjmp


.globl sigsetjmp
.type sigsetjmp, %function
sigsetjmp:
.size sigsetjmp, .-sigsetjmp
