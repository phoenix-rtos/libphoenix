/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * tls (sparc)
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
	register struct tls_tcb *tcb asm("g7");

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
