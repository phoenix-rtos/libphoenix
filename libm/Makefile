#
# Makefile for math library
#
# Copyright 2025 Phoenix Systems
# Author: Mikolaj Matalowski
#
# This file is part of Phoenix-RTOS.
#
# %LICENSE%
#
LIBM_USE_LIBMCS ?= n
LIBM_LIBMCS_DAZ ?= n
LIBM_WANT_COMPLEX ?= y

# Grab builtin definitions from gcc
GET_DEFINITIONS = $(shell $(CC) -dM -E - < /dev/null)
DEFINITIONS = $(filter-out #define, $(GET_DEFINITIONS))

# Setting sizes of floating point types
NAME_OF_TYPE_FOR_SIZE_CHECK := __SIZEOF_DOUBLE__
DOUBLE_SIZE = $(word 2, $(shell echo "$(DEFINITIONS)" | grep -E -o '$(NAME_OF_TYPE_FOR_SIZE_CHECK)\s+(\w+)'))

ifeq ($(DOUBLE_SIZE), 8)
  CPPFLAGS += -DLIBMCS_DOUBLE_IS_64BITS
else
  CPPFLAGS += -DLIBMCS_DOUBLE_IS_32BITS
endif

NAME_OF_TYPE_FOR_SIZE_CHECK := __SIZEOF_LONG_DOUBLE__
LDOUBLE_SIZE = $(word 2, $(shell echo "$(DEFINITIONS)" | grep -E -o '$(NAME_OF_TYPE_FOR_SIZE_CHECK)\s+(\w+)'))

ifeq ($(LDOUBLE_SIZE), 10)
  CPPFLAGS += -DLIBMCS_LONG_DOUBLE_IS_80BITS
else
  CPPFLAGS += -DLIBMCS_LONG_DOUBLE_IS_64BITS
endif

NAME_OF_TYPE_FOR_SIZE_CHECK := __SIZEOF_LONG__
LONG_SIZE = $(word 2, $(shell echo "$(DEFINITIONS)" | grep -E -o '$(NAME_OF_TYPE_FOR_SIZE_CHECK)\s+(\w+)'))

ifeq ($(LONG_SIZE), 8)
  CPPFLAGS += -DLIBMCS_LONG_IS_64BITS
else
  CPPFLAGS += -DLIBMCS_LONG_IS_32BITS
endif

# Set options for denormals on the FPU
ifeq ($(LIBMCS_WANT_DAZ), y)
  CPPFLAGS += -DLIBMCS_FPU_DAZ
endif

ifeq ($(LIBM_WANT_COMPLEX), y)
  CPPFLAGS += -DLIBMCS_WANT_COMPLEX
endif

CFLAGS += -Ilibm/libmcs/libm/include

# TODO: Separate math library implementation from libphoenix

# Build for libmcs
ifeq ($(USE_LIBMCS), y)
  ifeq ($(LIBM_WANT_COMPLEX), y)
    CPPFLAGS += -DLIBM_WANT_COMPLEX
    OBJS += $(addprefix $(PREFIX_O), $(patsubst %.c,%.o,$(wildcard libm/libmcs/libm/complexd/internal/*.c)))
    OBJS += $(addprefix $(PREFIX_O), $(patsubst %.c,%.o,$(wildcard libm/libmcs/libm/complexd/*.c)))
    OBJS += $(addprefix $(PREFIX_O), $(patsubst %.c,%.o,$(wildcard libm/libmcs/libm/complexf/internal/*.c)))
    OBJS += $(addprefix $(PREFIX_O), $(patsubst %.c,%.o,$(wildcard libm/libmcs/libm/complexf/*.c)))
  endif
  
  # fenv.c implementation is target specific
  OBJS += $(addprefix $(PREFIX_O), $(patsubst %.c,%.o, $(filter-out %fenv.c,$(wildcard libm/libmcs/libm/common/*.c))))
  OBJS += $(addprefix $(PREFIX_O), $(patsubst %.c,%.o,$(wildcard libm/libmcs/libm/mathf/*.c)))
  OBJS += $(addprefix $(PREFIX_O), $(patsubst %.c,%.o,$(wildcard libm/libmcs/libm/mathf/internal/*.c)))
  OBJS += $(addprefix $(PREFIX_O), $(patsubst %.c,%.o,$(wildcard libm/libmcs/libm/mathd/*.c)))
  OBJS += $(addprefix $(PREFIX_O), $(patsubst %.c,%.o,$(wildcard libm/libmcs/libm/mathd/internal/*.c)))
else
  # Build for phoenix implementation
  # OBJS += $(addprefix $(PREFIX_O)libm/, $(patsubst %.c,%.o, math/common.c math/trig.c))
  OBJS += $(addprefix $(PREFIX_O), $(patsubst %.c,%.o, $(patsubst %complex.c,, $(wildcard libm/phoenix/*.c))))

  ifeq ($(LIBM_WANT_COMPLEX), y)
    OBJS += $(addprefix $(PREFIX_O), $(patsubst %.c,%.o, libm/phoenix/complex.c))
  endif

  # This part is needed for libmcs headers compatibility
  CFLAGS += -include libm/libmcs/libm/include/internal_config.h
endif