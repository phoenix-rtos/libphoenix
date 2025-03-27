#
# Makefile for libphoenix/arch/arm
#
# Copyright 2024 Phoenix Systems
#
# %LICENSE%
#

# Obtain architecture based on ARM ACLE 2.0
# https://developer.arm.com/documentation/dui0774/l/Other-Compiler-specific-Features/Predefined-macros
arm_version := $(word 3, $(shell echo | arm-phoenix-gcc $(CFLAGS) -E -dM - | grep -E "[[:space:]]__ARM_ARCH[[:space:]]"))
arm_profile_num := $(word 3, $(shell echo | arm-phoenix-gcc $(CFLAGS) -E -dM - | grep -E "[[:space:]]__ARM_ARCH_PROFILE[[:space:]]"))
# ASCII conversion
arm_profile := $(subst 65,A,$(subst 77,M,$(subst 82,R,$(arm_profile_num))))
arm_arch := $(arm_version)$(arm_profile)

arm_use_common := y
ifneq (,$(findstring 7A,$(arm_arch)))
  include arch/arm/v7a/Makefile
else ifneq (,$(findstring 7M,$(arm_arch)))
  include arch/arm/v7m/Makefile
else ifneq (,$(findstring 8M,$(arm_arch)))
  include arch/arm/v8m/Makefile
else ifneq (,$(findstring 8R,$(arm_arch)))
  include arch/arm/v8r/Makefile
else ifeq ($(LIBPHOENIX_MULTILIB),y)
  include arch/arm/multilib/Makefile
  arm_use_common := n
else
  $(error Unsupported TARGET)
endif

# FIXME: multilib supports more targets that functions in those files do,
#        eg. arm/arch=armv5te or Thumb-1 targets (v6s-m and v8-m.base).
ifeq ($(arm_use_common),y)
OBJS += $(addprefix $(PREFIX_O)arch/arm/, jmp.o memcpy.o memset.o signal.o string.o)
endif

CRT0_OBJS += $(addprefix $(PREFIX_O)arch/arm/, crt0.o)
