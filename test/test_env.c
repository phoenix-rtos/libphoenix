/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * test/test_env
 *
 * Copyright 2017 Phoenix Systems
 * Author: Krystian Wasik
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define NUM_OF_VARIABLES        (64)
#define NUM_OF_TESTS            (1000)
#define MAX_NAME_LEN            (64)
#define MAX_VALUE_LEN           (32)

/* Defines how often environment should be cleared (approximately) */
#define CLEARENV_APPROX_EVERY   (200)

typedef struct {
	char name[MAX_NAME_LEN];
	char value[MAX_VALUE_LEN];
	char s[MAX_NAME_LEN + MAX_VALUE_LEN];
	unsigned set;
} env_var_t;

static env_var_t vars[NUM_OF_VARIABLES];

static int test_env_random(void);
static int test_env_exec_start(char *path);
static int test_env_exec_continue(void);

int main(int argc, char *argv[])
{
	int res;

	if (argc == 2 && strcmp(argv[1], "-e") == 0) {
		res = test_env_exec_continue();
		printf("test_env: exec test: %s\n", res == 0 ? "PASSED" : "FAILED");

	} else {
		res = test_env_random();
		printf("test_env: random test: %s\n", res == 0 ? "PASSED" : "FAILED");

		res = test_env_exec_start(argv[0]);
		printf("test_env: exec test: %s\n", res == 0 ? "PASSED" : "FAILED");
	}

	return 0;
}

static int test_env_random(void)
{
	int res;
	unsigned i, j, clearenv_cnt = 0, putenv_cnt = 0, setenv_cnt = 0, unsetenv_cnt = 0;

	srand(time(NULL));

	/* Initialize variables */
	for (i = 0; i < NUM_OF_VARIABLES; i++) {
		vars[i].set = 0;
		sprintf(vars[i].name, "VARIABLE%u", i);
		vars[i].value[0] = '\0';
	}

	/* Run tests */
	for (i = 0; i < NUM_OF_TESTS; i++) {

		printf("\rtest_env: random test: %u/%u", i + 1, NUM_OF_TESTS);

		if (rand()%CLEARENV_APPROX_EVERY == 0) {
			clearenv_cnt++;
			if ((res = clearenv()) != 0) {
				printf("\n\rclearenv() failed (%d)", res);
				return -1;
			}

			for (j = 0; j < NUM_OF_VARIABLES; j++) {
				vars[j].set = 0;
			}

		} else {
			unsigned idx = rand()%NUM_OF_VARIABLES;
			unsigned action = rand()%3;
			if (action == 2) { /* unsetenv */
				unsetenv_cnt++;
				if ((res = unsetenv(vars[idx].name)) != 0) {
					printf("\n\runsetenv(%s) failed (%d)", vars[idx].name, res);
					return -1;
				}

				vars[idx].set = 0;

			} else { /* insert (setenv or putenv) */

				/* Generate random value */
				unsigned len = rand()%MAX_VALUE_LEN;
				char new_value[MAX_VALUE_LEN];
				for (j = 0; j < len; j++) {
					char c;
					do {
						c = 33 + rand()%94;
					} while (c == '\0' || c == '=');
					new_value[j] = c;
				}
				new_value[len] = '\0';

				if (action) { /* setenv */
					unsigned overwrite = rand()%2;

					setenv_cnt++;
					if ((res = setenv(vars[idx].name, new_value, overwrite)) != 0) {
						printf("\n\rsetenv(%s, %s, %u) failed (%d)", vars[idx].name, new_value, overwrite, res);
						return -1;
					}

					if (!vars[idx].set || overwrite) strcpy(vars[idx].value, new_value);

				} else { /* putenv */
					strcpy(vars[idx].value, new_value);

					char *s = vars[idx].s;
					strcpy(s, vars[idx].name);
					s += strlen(vars[idx].name);
					*(s++) = '=';
					strcpy(s, vars[idx].value);

					putenv_cnt++;
					if ((res = putenv(vars[idx].s)) != 0) {
						printf("\n\rputenv(%s) failed (%d)", vars[idx].s, res);
						return -1;
					}
				}

				vars[idx].set = 1;
			}
		}

		/* Verify */
		for (j = 0; j < NUM_OF_VARIABLES; j++) {
			char *v = getenv(vars[j].name);
			if (!vars[j].set && v != NULL) {
				printf("\n\rgetenv returned value other than NULL for unset variable (%s)\n", vars[j].name);
				return -1;
			} else if (vars[j].set && (!v || strcmp(v, vars[j].value) != 0)) {
				printf("\n\rgetenv returned invalid value for set variable (%s)\n", vars[j].name);
				return -1;
			}
		}
	}

	printf("\n\rtest_env: random test: setenv: %u, putenv: %u, unsetenv: %u, clearenv: %u\n", setenv_cnt, putenv_cnt, unsetenv_cnt, clearenv_cnt);

	return 0;
}

extern char **environ;

#define EXEC_TEST_VAR           ("TEST_VARIABLE")
#define EXEC_TEST_VALUE         ("TEST_VALUE")

static int test_env_exec_start(char *path)
{
	if (setenv(EXEC_TEST_VAR, EXEC_TEST_VALUE, 1) != 0) {
		printf("test_env: exec test: setenv failed\n");
		return -1;
	}

	char *argv[3];
	argv[0] = path;
	argv[1] = "-e";
	argv[2] = NULL;

	if (execve(path, argv, environ) != 0) {
		printf("test_env: exec test: execve failed\n");
		return -1;
	}

	return 0;
}

static int test_env_exec_continue(void)
{
	char *v = getenv(EXEC_TEST_VAR);
	if (!v || strcmp(v, EXEC_TEST_VALUE) != 0) {
		printf("test_env: exec test: Test variable is not set or has wrong value.\n");
		return -1;
	}

	return 0;
}
