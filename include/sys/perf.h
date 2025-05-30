/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/perf
 *
 * Copyright 2025 Phoenix Systems
 * Author: Adam Greloch
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_SYS_PERF_H_
#define _LIBPHOENIX_SYS_PERF_H_

#include <phoenix/perf.h>

#ifdef __cplusplus
extern "C" {
#endif


extern int perf_start(perf_mode_t mode, unsigned flags, void *arg, size_t sz);


extern int perf_read(perf_mode_t mode, void *buffer, size_t bufsz, int chan);


extern int perf_stop(perf_mode_t mode);


extern int perf_finish(perf_mode_t mode);


#ifdef __cplusplus
}
#endif


#endif
