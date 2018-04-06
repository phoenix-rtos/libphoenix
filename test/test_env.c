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

int main(void)
{
    int res;

    res = test_env_random();
    printf("test_env: random test: %s\n", res == 0 ? "PASSED" : "FAILED");

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
                printf("%u/%u: clearenv() failed (%d)", i, NUM_OF_TESTS, res);
                return -1;
            }

            for (j = 0; j < NUM_OF_VARIABLES; j++) {
                vars[j].set = 0;
            }

        } else {
            unsigned idx = rand()%NUM_OF_VARIABLES;
            if (vars[idx].set) { /* unsetenv */
                unsetenv_cnt++;
                if ((res = unsetenv(vars[idx].name)) != 0) {
                    printf("%u/%u: unsetenv(%s) failed (%d)", i, NUM_OF_TESTS, vars[idx].name, res);
                    return -1;
                }

                vars[idx].set = 0;

            } else { /* insert (setenv or putenv) */

                /* Generate random value */
                unsigned len = rand()%MAX_VALUE_LEN;
                for (j = 0; j < len; j++) {
                    char c;
                    do {
                        c = 33 + rand()%94;
                    } while (c == '\0' || c == '=');
                    vars[idx].value[j] = c;
                }
                vars[idx].value[len] = '\0';

                unsigned action = rand()%2;
                if (action) { /* setenv */
                    setenv_cnt++;
                    if ((res = setenv(vars[idx].name, vars[idx].value, 1)) != 0) {
                        printf("%u/%u: setenv(%s, %s, 1) failed (%d)", i, NUM_OF_TESTS, vars[idx].name, vars[idx].value, res);
                        return -1;
                    }

                } else { /* putenv */
                    char *s = vars[idx].s;
                    strcpy(s, vars[idx].name);
                    s += strlen(vars[idx].name);
                    *(s++) = '=';
                    strcpy(s, vars[idx].value);

                    putenv_cnt++;
                    if ((res = putenv(vars[idx].s)) != 0) {
                        printf("%u/%u: putenv(%s) failed (%d)", i, NUM_OF_TESTS, vars[idx].s, res);
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
                printf("%u/%u: getenv returned value other than NULL for unset variable (%s)\n", i, NUM_OF_TESTS, vars[j].name);
                return -1;
            } else if (vars[j].set && (!v || strcmp(v, vars[j].value) != 0)) {
                printf("%u/%u: getenv returned invalid value for set variable (%s)\n", i, NUM_OF_TESTS, vars[j].name);
                return -1;
            }
        }
    }

    printf("\ntest_env: random test: setenv: %u, putenv: %u, unsetenv: %u, clearenv: %u\n", setenv_cnt, putenv_cnt, unsetenv_cnt, clearenv_cnt);

    return 0;
}
