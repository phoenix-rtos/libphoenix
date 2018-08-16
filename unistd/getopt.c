/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * unistd getopt
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski, Krystian Wasik
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <stdio.h>


char *optarg;


int optind = 1, opterr = 1, optopt;


static int optwhere = 1;


int getopt(int argc, char * const argv[], const char *optstring)
{
	char opt;
	char *optspec;
	int leading_colon;

	if (argc == 0 || argv == NULL || optind >= argc)
		return -1;

	if (argv[optind] == NULL || *argv[optind] != '-' || !strcmp(argv[optind], "-"))
		return -1;

	if (argv[optind][1] == '-') {
		optind++;
		return -1;
	}

	opt = argv[optind][optwhere++];
	leading_colon = *optstring == ':';

	optspec = strchr(optstring, opt);
	if (optspec == NULL) { /* Unrecognized option */
		optopt = opt;
		if (opterr && !leading_colon)
			fprintf(stderr, "%s: illegal option -- %c\n", argv[0], opt);
		if (argv[optind][optwhere] == '\0') {
			optind++;
			optwhere = 1;
		}
		return '?';
	}

	if (*(++optspec) == ':') {
		if (argv[optind][optwhere] != '\0') { /* Argument in the same argv */
			optarg = &argv[optind][optwhere];

		} else if (optind + 1 < argc) { /* Argument in the next argv */
			optarg = argv[optind + 1];
			optind ++;

		} else { /* Argument not provided */
			optopt = opt;
			optind++;
			optwhere = 1;
			if (leading_colon) {
				return ':';
			} else {
				if (opterr)
					fprintf(stderr, "%s: option requires an argument -- %c\n", argv[0], opt);
				return '?';
			}
		}

	} else if (argv[optind][optwhere] != '\0') { /* More options in the same argv */
		return opt;
	}

	optwhere = 1;
	optind++;

	return opt;
}


int getopt_long(int argc, char * const argv[], const char *optstring, const struct option *longopts, int *longindex)
{
	return -1;
}
