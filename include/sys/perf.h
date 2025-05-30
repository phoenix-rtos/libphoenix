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
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


int perf_start(perf_mode_t mode, unsigned int flags, void *arg, size_t sz);


int perf_read(perf_mode_t mode, void *buffer, size_t bufsz, int chan);


int perf_stop(perf_mode_t mode);


int perf_finish(perf_mode_t mode);


#ifdef __cplusplus
}
#endif


#endif
