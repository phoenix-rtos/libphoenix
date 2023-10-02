/*
 * Phoenix-RTOS
 *
 * ubsan
 *
 * Undefined Behavior Sanitization
 *
 * Copyright 2023 Phoenix Systems
 * Author: Gerard Swiderski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <unistd.h>


struct source_location {
	const char *file;
	uint32_t line;
	uint32_t column;
};


struct type_descriptor {
	uint16_t type_kind;
	uint16_t type_info;
	char *type_name;
};


struct type_mismatch_data {
	struct source_location loc;
	struct type_descriptor *type;
	unsigned long alignment;
	unsigned char type_check_kind;
};


struct type_mismatch_data_v1 {
	struct source_location loc;
	struct type_descriptor *type;
	unsigned char alignment;
	unsigned char type_check_kind;
};


struct overflow_data {
	struct source_location loc;
	struct type_descriptor *type;
};


struct shift_out_of_bounds_data {
	struct source_location loc;
	struct type_descriptor *lhs_type;
	struct type_descriptor *rhs_type;
};


struct out_of_bounds_data {
	struct source_location loc;
	struct type_descriptor *array_type;
	struct type_descriptor *index_type;
};


struct unreachable_data {
	struct source_location loc;
};


struct vla_bound_data {
	struct source_location loc;
	struct type_descriptor *type;
};


struct invalid_value_data {
	struct source_location loc;
	struct type_descriptor *type;
};


struct nonnull_arg_data {
	struct source_location loc;
};


struct nonnull_return_data {
	struct source_location loc;
	struct source_location attr_loc;
};


struct invalid_builtin_data {
	struct source_location loc;
	uint8_t kind;
};


struct pointer_overflow_data {
	struct source_location loc;
};


static void ubsan_print(struct source_location *loc, const char *message)
{
	const char *fmt = (isatty(STDERR_FILENO) != 0) ?
		"\033[1m%s:%" PRIu32 ":%" PRIu32 ": \033[31mruntime error:\033[0;1m %s\033[0m\n" :
		"%s:%" PRIu32 ":%" PRIu32 ": runtime error: %s\n";

	(void)fprintf(stderr, fmt, loc->file, loc->line, loc->column, message);
}


void __ubsan_handle_add_overflow(struct overflow_data *data)
{
	ubsan_print(&data->loc, "addition overflow");
}


void __ubsan_handle_sub_overflow(struct overflow_data *data)
{
	ubsan_print(&data->loc, "subtraction overflow");
}


void __ubsan_handle_mul_overflow(struct overflow_data *data)
{
	ubsan_print(&data->loc, "multiplication overflow");
}


void __ubsan_handle_divrem_overflow(struct overflow_data *data)
{
	ubsan_print(&data->loc, "division overflow");
}


void __ubsan_handle_negate_overflow(struct overflow_data *data)
{
	ubsan_print(&data->loc, "negation overflow");
}


void __ubsan_handle_pointer_overflow(struct overflow_data *data)
{
	ubsan_print(&data->loc, "pointer overflow");
}


void __ubsan_handle_shift_out_of_bounds(struct shift_out_of_bounds_data *data)
{
	ubsan_print(&data->loc, "shift out of bounds");
}


void __ubsan_handle_out_of_bounds(struct out_of_bounds_data *data)
{
	ubsan_print(&data->loc, "array out of bounds");
}


void __ubsan_handle_load_invalid_value(struct invalid_value_data *data)
{
	ubsan_print(&data->loc, "invalid load value");
}


void __ubsan_handle_type_mismatch(struct type_mismatch_data *data, uintptr_t ptr)
{
	if (ptr == (uintptr_t)NULL) {
		ubsan_print(&data->loc, "null pointer access");
	}
	else if ((data->alignment != 0) && ((ptr & (((uintptr_t)1 << data->alignment) - (uintptr_t)1)) != 0)) {
		ubsan_print(&data->loc, "misaligned pointer is used");
	}
	else {
		ubsan_print(&data->loc, "insufficient space for an object");
	}
}


void __ubsan_handle_type_mismatch_v1(struct type_mismatch_data_v1 *data, uintptr_t ptr)
{
	if (ptr == (uintptr_t)NULL) {
		ubsan_print(&data->loc, "null pointer access");
	}
	else if ((data->alignment != 0) && ((ptr & (((uintptr_t)1 << data->alignment) - (uintptr_t)1)) != 0)) {
		ubsan_print(&data->loc, "misaligned pointer is used");
	}
	else {
		ubsan_print(&data->loc, "insufficient space for an object");
	}
}


void __ubsan_handle_vla_bound_not_positive(struct vla_bound_data *data)
{
	ubsan_print(&data->loc, "variable-length argument out of bounds");
}


void __ubsan_handle_nonnull_return(struct nonnull_return_data *data)
{
	ubsan_print(&data->loc, "null pointer returned from a function 'nonnull' specified");
}


void __ubsan_handle_nonnull_return_v1(struct nonnull_return_data *data, struct source_location *loc)
{
	ubsan_print(&data->loc, "null pointer returned from a function 'nonnull' specified");
}


#if __GCC_VERSION < 60000
void __ubsan_handle_nonnull_arg(struct nonnull_arg_data *data, size_t arg_no)
#else
void __ubsan_handle_nonnull_arg(struct nonnull_arg_data *data)
#endif
{
	ubsan_print(&data->loc, "non-null argument is null");
}


void __ubsan_handle_builtin_unreachable(struct unreachable_data *data)
{
	ubsan_print(&data->loc, "unreachable code reached");
}


void __ubsan_handle_invalid_builtin(struct invalid_builtin_data *data)
{
	ubsan_print(&data->loc, "invalid builtin");
}


void __ubsan_handle_missing_return(struct unreachable_data *data)
{
	ubsan_print(&data->loc, "missing return value");
}
