	.global	ph_syscall
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	ph_syscall, %function
ph_syscall:
	@ args = 4, pretend = 16, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 1
	@ link register save eliminated.
	push	{r0, r1, r2, r3}
	movs	r3, #0
	mov	r0, r3
	add	sp, sp, #16
	bx	lr
	.size	ph_syscall, .-ph_syscall
