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


#endif /* _LIBPHOENIX_SYS_CDEFS_H */
