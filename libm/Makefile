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

# Set options for denormals on the FPU
ifeq ($(LIBM_LIBMCS_DAZ), y)
  CPPFLAGS += -DLIBMCS_FPU_DAZ
endif

ifeq ($(LIBM_WANT_COMPLEX), y)
  CPPFLAGS += -DLIBMCS_WANT_COMPLEX
endif

CFLAGS += -Ilibm/libmcs/libm/include

# TODO: Separate math library implementation from libphoenix

# Build for libmcs
ifeq ($(LIBM_USE_LIBMCS), y)
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