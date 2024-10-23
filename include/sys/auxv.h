/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/auxv.h
 *
 * Copyright 2024 Phoenix Systems
 * Author: Hubert Badocha
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_SYS_AUXV_H
#define _LIBPHOENIX_SYS_AUXV_H

#include <phoenix/auxv.h>
#include <stdint.h>


uint64_t getauxval(uint32_t type);


#endif /* _LIBPHOENIX_SYS_AUXV_H */
