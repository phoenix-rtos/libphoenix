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
endif

OBJS += $(addprefix $(PREFIX_O)arch/arm/, jmp.o memcpy.o memset.o signal.o string.o)
CRT0_OBJS += $(addprefix $(PREFIX_O)arch/arm/, crt0.o)
