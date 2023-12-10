/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Signal trampoline (ia32)
 *
 * Copyright 2019, 2023 Phoenix Systems
 * Author: Jan Sikorski, Hubert Badocha
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#define __ASSEMBLY__

.text

.globl _signal_trampoline
.type _signal_trampoline, %function
_signal_trampoline:
	/* Signal number on stack */
#if __pic__ == 0
	call  _signal_handler
#else
	/* ebx will be restored by sigreturn */
	call  .l1
.l1:
	popl  %ebx
	addl  $_GLOBAL_OFFSET_TABLE_+(.-.l1), %ebx

	call  _signal_handler@got(%ebx)
#endif

	addl  $4, %esp
	/* Put old mask on stack */
	pushl %eax
	/* cpu context *, eip, esp on stack */
#if __pic__ == 0
	call  sigreturn
#else
	call  sigreturn@got(%ebx)
#endif
.size _signal_trampoline, .-_signal_trampoline

