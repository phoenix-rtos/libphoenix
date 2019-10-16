/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/minmax.h
 *
 * Copyright 2019 Phoenix Systems
 * Author: Andrzej Glowinski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _SYS_MINMAX_H_
#define _SYS_MINMAX_H_

#define max(a, b) ({ \
  __typeof__ (a) _a = (a); \
  __typeof__ (b) _b = (b); \
  _a > _b ? _a : _b; })

#define min(a, b) ({ \
  __typeof__ (a) _a = (a); \
  __typeof__ (b) _b = (b); \
        _a > _b ? _b : _a; \
})

#endif

