#
# Makefile for math library
#
# Copyright 2025-2026 Phoenix Systems
# Author: Mikolaj Matalowski
#
# This file is part of Phoenix-RTOS.
#
# SPDX-License-Identifier: BSD-3-Clause
#

LIBM_USE_LIBMCS ?= n
LIBM_LIBMCS_DAZ ?= n
LIBM_WANT_COMPLEX ?= y
LIBM_USE_HW ?= y

# Set options for denormals on the FPU
ifeq ($(LIBM_LIBMCS_DAZ), y)
  CPPFLAGS += -DLIBMCS_FPU_DAZ
endif

ifeq ($(LIBM_WANT_COMPLEX), y)
  CPPFLAGS += -DLIBMCS_WANT_COMPLEX
endif

CFLAGS += -Ilibm/libmcs/libm/include

# Add source files for hardware accelerated math
HW_OBJS :=
ifeq ($(LIBM_USE_HW), y)
  include libm/arch/Makefile
endif

SW_OBJS := 
# Build for libmcs
ifeq ($(LIBM_USE_LIBMCS), y)
  ifeq ($(LIBM_WANT_COMPLEX), y)
    CPPFLAGS += -DLIBM_WANT_COMPLEX
    SW_OBJS += $(addprefix $(PREFIX_O), $(patsubst %.c,%.o,$(wildcard libm/libmcs/libm/complexd/internal/*.c)))
    SW_OBJS += $(addprefix $(PREFIX_O), $(patsubst %.c,%.o,$(wildcard libm/libmcs/libm/complexd/*.c)))
    SW_OBJS += $(addprefix $(PREFIX_O), $(patsubst %.c,%.o,$(wildcard libm/libmcs/libm/complexf/internal/*.c)))
    SW_OBJS += $(addprefix $(PREFIX_O), $(patsubst %.c,%.o,$(wildcard libm/libmcs/libm/complexf/*.c)))
  endif
  
  # fenv.c implementation is target specific
  SW_OBJS += $(addprefix $(PREFIX_O), $(patsubst %.c,%.o, $(filter-out %fenv.c,$(wildcard libm/libmcs/libm/common/*.c))))
  SW_OBJS += $(addprefix $(PREFIX_O), $(patsubst %.c,%.o,$(wildcard libm/libmcs/libm/mathf/*.c)))
  SW_OBJS += $(addprefix $(PREFIX_O), $(patsubst %.c,%.o,$(wildcard libm/libmcs/libm/mathf/internal/*.c)))
  SW_OBJS += $(addprefix $(PREFIX_O), $(patsubst %.c,%.o,$(wildcard libm/libmcs/libm/mathd/*.c)))
  SW_OBJS += $(addprefix $(PREFIX_O), $(patsubst %.c,%.o,$(wildcard libm/libmcs/libm/mathd/internal/*.c)))
else
  ifeq ($(LIBM_WANT_COMPLEX), y)
    SW_OBJS += $(addprefix $(PREFIX_O), $(patsubst %.c,%.o, libm/phoenix/complex.c))
  endif

  # Build for phoenix implementation
  SW_OBJS += $(addprefix $(PREFIX_O), $(patsubst %.c,%.o, $(patsubst %complex.c,, $(wildcard libm/phoenix/*.c))))
  SW_OBJS += $(addprefix $(PREFIX_O), $(patsubst %.c,%.o,$(wildcard libm/phoenix/mathd/*.c)))
  SW_OBJS += $(addprefix $(PREFIX_O), $(patsubst %.c,%.o,$(wildcard libm/phoenix/mathf/*.c)))
  
  CFLAGS += -Ilibm/phoenix
  # This part is needed for libmcs headers compatibility
  CFLAGS += -include libm/libmcs/libm/include/internal_config.h
endif

# Clear functions that already implemented in hardware
OBJS += $(foreach f,$(SW_OBJS),$(if $(filter $(notdir $(f)),$(notdir $(HW_OBJS))),,$(f)))
OBJS += $(HW_OBJS)