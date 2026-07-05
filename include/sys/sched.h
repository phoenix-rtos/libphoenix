/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/sched
 *
 * Copyright 2026 Phoenix Systems
 * Author: Adam Greloch
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _SYS_SCHED_H_
#define _SYS_SCHED_H_


#include <phoenix/sched.h>


#ifdef __cplusplus
extern "C" {
#endif


int schedInfo(int pid, int policy, sched_info_t *info);


int schedGet(int pid, int tid, sched_params_t *params);


int schedSet(int pid, int tid, int policy, sched_params_t *params);


#ifdef __cplusplus
}
#endif


#endif
