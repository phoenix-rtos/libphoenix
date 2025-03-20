#
# Makefile for libphoenix/arch/arm
#
# Copyright 2024 Phoenix Systems
#
# %LICENSE%
#

ifneq (,$(findstring v7a,$(TARGET_SUFF)))
  include arch/arm/v7a/Makefile
else ifneq (,$(findstring v7m,$(TARGET_SUFF)))
  include arch/arm/v7m/Makefile
else ifneq (,$(findstring v8m,$(TARGET_SUFF)))
  include arch/arm/v8m/Makefile
else ifneq (,$(findstring v8r,$(TARGET_SUFF)))
  include arch/arm/v8r/Makefile
else ifneq (,$(findstring v8r,$(TARGET_SUFF)))
  include arch/arm/v8r/Makefile
else ifeq ($(TARGET_SUBFAMILY),multilib)
  # TODO: Try to include correct Makefile if possible in case of a multilib.
  include arch/arm/multilib/Makefile
else
  $(error Unsupported TARGET)
endif

# FIXME: multilib supports more targets that functions in those files do,
#        eg. arm/arch=armv5te or Thumb-1 targets (v6s-m and v8-m.base).
ifneq ($(TARGET_SUBFAMILY),multilib)
OBJS += $(addprefix $(PREFIX_O)arch/arm/, jmp.o memcpy.o memset.o signal.o string.o)
endif

CRT0_OBJS += $(addprefix $(PREFIX_O)arch/arm/, crt0.o)
