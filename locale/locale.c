/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * locale.c
 *
 * Copyright 2018 Phoenix Systems
 * Author: Michal Miroslaw
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */
#include "locale.h"

char *_PathLocale;

const struct lconv _fixed_locale = {
	"."
};
