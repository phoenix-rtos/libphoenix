/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * locale.c
 *
 * Copyright 2018, 2024 Phoenix Systems
 * Author: Michal Miroslaw
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */
#include <limits.h>
#include <locale.h>

/* for regex/collate.c */
char *_PathLocale;


static const struct lconv fixed_locale = {
	.decimal_point = ".",
	.thousands_sep = "",
	.grouping = "",
	.mon_decimal_point = "",
	.mon_thousands_sep = "",
	.mon_grouping = "",
	.positive_sign = "",
	.negative_sign = "",
	.currency_symbol = "",
	.int_curr_symbol = "",
	.frac_digits = CHAR_MAX,
	.p_cs_precedes = CHAR_MAX,
	.n_cs_precedes = CHAR_MAX,
	.p_sep_by_space = CHAR_MAX,
	.n_sep_by_space = CHAR_MAX,
	.p_sign_posn = CHAR_MAX,
	.n_sign_posn = CHAR_MAX,
	.int_frac_digits = CHAR_MAX,
	.int_p_cs_precedes = CHAR_MAX,
	.int_n_cs_precedes = CHAR_MAX,
	.int_p_sep_by_space = CHAR_MAX,
	.int_n_sep_by_space = CHAR_MAX,
	.int_p_sign_posn = CHAR_MAX,
	.int_n_sign_posn = CHAR_MAX,
};


struct lconv *localeconv(void)
{
	return (struct lconv *)&fixed_locale;
}
