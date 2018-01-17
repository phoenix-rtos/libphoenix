/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * unistd getopt
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <unistd.h>
#include <getopt.h>


char *optarg;
int optind, opterr, optopt;


int getopt(int argc, char * const argv[], const char *optstring)
{
	return -1;
}


int getopt_long(int argc, char * const argv[], const char *optstring, const struct option *longopts, int *longindex)
{
	return -1;
}
