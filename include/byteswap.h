/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * byte swapping utilities
 *
 * Copyright 2017, 2018 Phoenix Systems
 * Author: Marek Bialowas
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */


#ifndef _BYTESWAP_H
#define _BYTESWAP_H	1


/* Return a value with all bytes in the 16 bit argument swapped.  */
#define bswap_16(x) __builtin_bswap16 (x)

/* Return a value with all bytes in the 32 bit argument swapped.  */
#define bswap_32(x) __builtin_bswap32 (x)

/* Return a value with all bytes in the 64 bit argument swapped.  */
#define bswap_64(x) __builtin_bswap64 (x)

#endif /* byteswap.h */
