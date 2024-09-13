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

#include <sys/auxv.h>


extern char **auxvStart;


uint64_t getauxval(uint32_t type)
{
	for (struct auxInfo *ai = (struct auxInfo *)auxvStart; ai->a_type != AT_NULL; ai++) {
		if (ai->a_type == type) {
			return ai->a_v;
		}
	}

	return 0;
}
