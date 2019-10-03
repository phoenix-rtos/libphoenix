/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * endian checks and conversions
 *
 * Copyright 2017, 2018 Phoenix Systems
 * Author: Michał Mirosław
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_ENDIAN_H_
#define _LIBPHOENIX_ENDIAN_H_

#include <stdint.h>


#define __LITTLE_ENDIAN 0
#define __BIG_ENDIAN 1

#include <arch.h>

#if __BYTE_ORDER == __LITTLE_ENDIAN
#define __swap_le u
#define __swap_be __builtin_bswap
#else
#define __swap_le __builtin_bswap
#define __swap_be u
#endif


#define __CPP_CONCAT1(x, y) x ## y
#define __CPP_CONCAT(x, y) __CPP_CONCAT1(x, y)


#define __swap_le__(b) __CPP_CONCAT(__swap_le, b)
#define __swap_be__(b) __CPP_CONCAT(__swap_be, b)


#define le16toh(x) (__swap_le__(16))(x)
#define le32toh(x) (__swap_le__(32))(x)
#define le64toh(x) (__swap_le__(64))(x)
#define htole16(x) (__swap_le__(16))(x)
#define htole32(x) (__swap_le__(32))(x)
#define htole64(x) (__swap_le__(64))(x)


#define be16toh(x) (__swap_be__(16))(x)
#define be32toh(x) (__swap_be__(32))(x)
#define be64toh(x) (__swap_be__(64))(x)
#define htobe16(x) (__swap_be__(16))(x)
#define htobe32(x) (__swap_be__(32))(x)
#define htobe64(x) (__swap_be__(64))(x)


#endif /* _LIBPHOENIX_ENDIAN_H_ */
