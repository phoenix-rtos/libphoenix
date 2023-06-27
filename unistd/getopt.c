/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * unistd getopt
 *
 * Copyright 2018, 2020 Phoenix Systems
 * Author: Jan Sikorski, Krystian Wasik, Lukasz Kosinski
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

	if (optind <= 0) {
		optind = 1;
		optwhere = 1;
	}

	if ((argc == 0) || (argv == NULL) || (optind >= argc)) {
		return -1;
	}

	if ((argv[optind] == NULL) || (argv[optind][0] != '-') || (argv[optind][1] == '\0')) {
		return -1;
	}

	if ((argv[optind][1] == '-') && (argv[optind][2] == '\0')) {
		optind++;
		return -1;
	}

	opt = argv[optind][optwhere++];
	leading_colon = ((*optstring == ':') ? 1 : 0);

	optspec = strchr(optstring, opt);
	if (optspec == NULL) {
		/* Unrecognized option */
		optopt = opt;
		if ((opterr != 0) && (leading_colon == 0)) {
			fprintf(stderr, "%s: illegal option -- %c\n", argv[0], opt);
		}
		if (argv[optind][optwhere] == '\0') {
			optind++;
			optwhere = 1;
		}
		return '?';
	}

	if (*(++optspec) == ':') {
		if (argv[optind][optwhere] != '\0') {
			/* Argument in the same argv */
			optarg = &argv[optind][optwhere];
		}
		else if (*(++optspec) == ':') {
			/* Optional argument */
			optarg = NULL;
		}
		else if (optind + 1 < argc) {
			/* Argument in the next argv */
			optarg = argv[optind + 1];
			optind++;
		}
		else {
			/* Argument not provided */
			optopt = opt;
			optind++;
			optwhere = 1;
			if (leading_colon != 0) {
				return ':';
			}
			else {
				if (opterr != 0) {
					fprintf(stderr, "%s: option requires an argument -- %c\n", argv[0], opt);
				}
				return '?';
			}
		}
	}
	else if (argv[optind][optwhere] != '\0') {
		/* More options in the same argv */
		return opt;
	}

	optwhere = 1;
	optind++;

	return opt;
}


static void rotate_right(char **dest, char **src)
{
	char *tmp = *src;

	memmove(dest + 1, dest, (src - dest) * sizeof(char *));
	*dest = tmp;
}


int getopt_long(int argc, char * const argv[], const char *optstring, const struct option *longopts, int *longindex)
{
	char *name, *opt = NULL;
	int skipped, resumed;
	int i, colon, ret;

	if (argc == 0 || argv == NULL)
		return -1;

	skipped = optind;
	if (optstring[0] != '+' && optstring[0] != '-') {
		for (i = optind; ; i++) {
			if (i >= argc || argv[i] == NULL)
				return -1;
			if (argv[i][0] == '-' && argv[i][1])
				break;
		}
		optind = i;
	}
	resumed = optind;

	optarg = NULL;
	if (longopts && argv[optind] != NULL && argv[optind][0] == '-' && argv[optind][1] == '-' && argv[optind][2]) {
		colon = optstring[optstring[0] == '+' || optstring[0] == '-'] == ':';

		for (i = 0; (name = (char *)longopts[i].name); i++) {
			opt = argv[optind] + 2;
			for (; *name && *name == *opt; name++, opt++);
			if (*opt && *opt != '=')
				continue;
			if (!*name)
				break;
		}

		if (name != NULL) {
			optind++;
			optopt = longopts[i].val;

			if (*opt == '=') {
				if (!longopts[i].has_arg) {
					if (!colon && opterr)
						fprintf(stderr, "%s: option does not take an argument -- %s\n", argv[0], longopts[i].name);
					return '?';
				}
				optarg = opt + 1;
			}
			else if (longopts[i].has_arg == required_argument){
				if (!(optarg = argv[optind])) {
					if (colon)
						return ':';
					if (opterr)
						fprintf(stderr, "%s: option requires an argument -- %s\n", argv[0], longopts[i].name);
					return '?';
				}
				optind++;
			}

			if (longindex)
				*longindex = i;
			if (longopts[i].flag) {
				*longopts[i].flag = longopts[i].val;
				return 0;
			}
			return longopts[i].val;
		}

		if (!colon && opterr)
			fprintf(stderr, "%s: illegal option -- %s\n", argv[0], argv[optind] + 2);
		optind++;
		return '?';
	}
	ret = getopt(argc, argv, optstring);

	if (resumed > skipped) {
		for (i = 0; i < optind - resumed; i++)
			rotate_right((char **)argv + skipped, (char **)argv + optind - 1);
		optind -= resumed - skipped;
	}

	return ret;
}
