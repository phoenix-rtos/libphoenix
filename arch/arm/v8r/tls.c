/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * tls (armv8r)
 *
 * Copyright 2024 Phoenix Systems
 * Author: Hubert Badocha
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */


#include <sys/tls.h>


struct tls_tcb *__tls_getTcb(void)
{
	void *tcb;
	/* clang-format off */
	__asm__ volatile("mrc p15, 0, %0, cr13, cr0, 3" : "=r"(tcb));
	/* clang-format on */
	return tcb;
}


void *__tls_tcbPtrToTlsPtr(struct tls_tcb *tcb)
{
	return (void *)tcb;
}


void __tls_archInit(void)
{
	/* Nothing to do. */
}
