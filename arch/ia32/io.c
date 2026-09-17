/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * I/O functions (ia32)
 *
 * Copyright 2026 Phoenix Systems
 * Author: Adam Greloch
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <sys/io.h>


__EXPORT_INLINE unsigned char inb(void *addr);
__EXPORT_INLINE void outb(void *addr, unsigned char b);
__EXPORT_INLINE unsigned short inw(void *addr);
__EXPORT_INLINE void outw(void *addr, unsigned short w);
__EXPORT_INLINE unsigned int inl(void *addr);
__EXPORT_INLINE void outl(void *addr, unsigned int l);
