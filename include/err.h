/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * err.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Michal Miroslaw
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_ERR_H_
#define _LIBPHOENIX_ERR_H_


#include <stdarg.h>


#ifdef __cplusplus
extern "C" {
#endif


extern void warn(const char *fmt, ...) __attribute__ ((format(printf, 1, 2)));
extern void warnx(const char *fmt, ...) __attribute__ ((format(printf, 1, 2)));
extern void err(int status, const char *fmt, ...) __attribute__ ((noreturn, format(printf, 2, 3)));
extern void errx(int status, const char *fmt, ...) __attribute__ ((noreturn, format(printf, 2, 3)));

extern void vwarn(const char *fmt, va_list va) __attribute__ ((format(printf, 1, 0)));
extern void vwarnx(const char *fmt, va_list va) __attribute__ ((format(printf, 1, 0)));
extern void verr(int status, const char *fmt, va_list va) __attribute__ ((noreturn, format(printf, 2, 0)));
extern void verrx(int status, const char *fmt, va_list va) __attribute__ ((noreturn, format(printf, 2, 0)));


#ifdef __cplusplus
}
#endif


#endif /* _LIBPHOENIX_ERR_H_ */
