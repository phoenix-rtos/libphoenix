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


#ifdef __cplusplus
extern "C" {
#endif


struct option {
	const char *name;
	int         has_arg;
	int        *flag;
	int         val;
};


enum { no_argument = 0, required_argument, optional_argument };


extern int getopt_long(int argc, char * const argv[], const char *optstring, const struct option *longopts, int *longindex);


#ifdef __cplusplus
}
#endif


#endif
