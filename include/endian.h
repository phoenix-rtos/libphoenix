/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * endian checks and conversions
 *
 * Copyright 2017, 2018, 2020 Phoenix Systems
 * Author: Michal Miroslaw, Lukasz Kosinski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_ENDIAN_H_
#define _LIBPHOENIX_ENDIAN_H_


#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

#define __BYTE_ORDER    __BYTE_ORDER__
#define __LITTLE_ENDIAN __ORDER_LITTLE_ENDIAN__
#define __BIG_ENDIAN    __ORDER_BIG_ENDIAN__

#include <arch.h>

#define __CPP_CONCAT1(x, y) x##y
#define __CPP_CONCAT(x, y)  __CPP_CONCAT1(x, y)


#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define __swap_le__(b) __CPP_CONCAT(__CPP_CONCAT(uint, b), _t)
#define __swap_be__(b) __CPP_CONCAT(__builtin_bswap, b)
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define __swap_le__(b) __CPP_CONCAT(__builtin_bswap, b)
#define __swap_be__(b) __CPP_CONCAT(__CPP_CONCAT(uint, b), _t)
#else
#error "Unsupported byte order"
#endif


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

/* Network byte order conversions (aliases for big-endian conversions) */
/* Note: ntohs is defined as a function in inet.c, not a macro */
#include <stdint.h>
extern uint16_t ntohs(uint16_t netshort);
#ifndef ntohl
#define ntohl(x) be32toh(x)
#endif
#ifndef htons
#define htons(x) htobe16(x)
#endif
#ifndef htonl
#define htonl(x) htobe32(x)
#endif


#ifdef __cplusplus
}
#endif


#endif /* _LIBPHOENIX_ENDIAN_H_ */
