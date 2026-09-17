/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/cdefs.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Michal Miroslaw
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_SYS_CDEFS_H
#define _LIBPHOENIX_SYS_CDEFS_H

#define __GLIBC_PREREQ(x,y) 0

#define __LEAF , __leaf__
#define __LEAF_ATTR __attribute__ ((__leaf__))

#ifndef __cplusplus
#  define __THROW       __attribute__ ((__nothrow__ __LEAF))
#  define __THROWNL     __attribute__ ((__nothrow__))
#  define __NTH(fct)    __attribute__ ((__nothrow__ __LEAF)) fct
#else
#  define __THROW      throw ()
#  define __THROWNL    throw ()
#  define __NTH(fct)   __LEAF_ATTR fct throw ()
#endif


#ifndef	__cplusplus
#  define __BEGIN_DECLS
#  define __END_DECLS
#else
#  define __BEGIN_DECLS	extern "C" {
#  define __END_DECLS	}
#endif


/*
 * Macros for gnu89/gnu99/GCC-builtin non-conflicting way of declaring
 * inline functions with external linkage in headers.
 *
 * Use __INLINE in the headers, then add __EXPORT_INLINE declaration
 * in exactly one .c file.
 */
#define __INLINE        extern __inline __attribute__((__gnu_inline__))
#define __EXPORT_INLINE __inline __attribute__((__gnu_inline__))


#endif /* _LIBPHOENIX_SYS_CDEFS_H */
