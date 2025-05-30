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

#include <sys/perf.h>

extern int sys_perf_start(int mode, unsigned int flags, void *arg, size_t sz);
extern int sys_perf_read(int mode, void *buffer, size_t bufsz, int chan);
extern int sys_perf_stop(int mode);
extern int sys_perf_finish(int mode);


int perf_start(perf_mode_t mode, unsigned int flags, void *arg, size_t sz)
{
	return sys_perf_start((int)mode, flags, arg, sz);
}


int perf_read(perf_mode_t mode, void *buffer, size_t bufsz, int chan)
{
	return sys_perf_read((int)mode, buffer, bufsz, chan);
}


int perf_stop(perf_mode_t mode)
{
	return sys_perf_stop((int)mode);
}


int perf_finish(perf_mode_t mode)
{
	return sys_perf_finish((int)mode);
}
