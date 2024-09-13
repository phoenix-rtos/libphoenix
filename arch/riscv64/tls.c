/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * tls (riscv64)
 *
 * Copyright 2024 Phoenix Systems
 * Author: Hubert Badocha
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */


#include <sys/tls.h>


/* As per RISC-V ABI:
 * RISC-V uses Variant I as described by the ELF TLS specification, with tp containing the address one past the end of the TCB */
struct tls_tcb *__tls_getTcb(void)
{
	void *tcb;
	/* clang-format off */
	__asm__ volatile("addi %[tcb], tp, %[offset]"
					 : [tcb] "=r"(tcb)
					 : [offset] "n"(-sizeof(struct tls_tcb)));
	/* clang-format on */
	return tcb;
}


void *__tls_tcbPtrToTlsPtr(struct tls_tcb *tcb)
{
	return (void *)(tcb + 1);
}


void __tls_archInit(void)
{
	/* Nothing to do. */
}
