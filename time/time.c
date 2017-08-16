/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * time
 *
 * Copyright 2017 Phoenix Systems
 * Author: Andrzej Asztemborski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include ARCH

#include "time.h"

time_t time(time_t *tp) {
	time_t now;

	gettime(&now);
	now /= 1000; /* milliseconds to seconds */

	if (tp != NULL)
		*tp = now;

	return now;
}
