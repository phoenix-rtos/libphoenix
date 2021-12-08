/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * getopt.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_GETOPT_H_
#define _LIBPHOENIX_GETOPT_H_

#include <unistd.h>


/* Long option argument */
enum { no_argument = 0, required_argument, optional_argument };


struct option {
	const char *name; /* Long option name */
	int has_arg;      /* Long option argumnent */
	int *flag;        /* Specifies how results are returned for a long option */
	int val;          /* Value to return */
};


extern int getopt_long(int argc, char * const argv[], const char *optstring, const struct option *longopts, int *longindex);


#endif
