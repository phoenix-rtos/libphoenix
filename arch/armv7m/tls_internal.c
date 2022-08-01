/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * tls_internal.c
 *
 * Copyright 2022 Phoenix Systems
 * Author: Lukasz Leczkowski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <sys/types.h>


volatile uintptr_t arm_tls_ptr __attribute__((section("armtls"))) = 0;


void aeabi_read_tp_internal(void)
{
	__asm__ volatile("mov r0,%[tls]"
					 :
					 : [tls] "r"(arm_tls_ptr)
					 :);
}
