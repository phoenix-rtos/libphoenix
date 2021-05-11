# Makefile for Phoenix-RTOS 3
#
# Copyright 2021 Phoenix Systems
#

# checking allowed TARGETs

# ARM Cortex Mx
TARGETS_ARMCORTEXM := \
	armv7m3-stm32l152xd \
	armv7m3-stm32l152xe \
	armv7m4-stm32l4x6 \
	armv7m7-imxrt105x \
	armv7m7-imxrt106x \
	armv7m7-imxrt117x

TARGETS := $(TARGETS_ARMCORTEXM)
ifneq (,$(filter $(TARGETS_ARMCORTEXM),$(TARGET_FAMILY)-$(TARGET_SUBFAMILY)))
  TARGET_SUFF ?= armv7m
endif

# ARM Cortex Ax
TARGETS_ARMCORTEXA := \
	armv7a7-imx6ull \
	armv7a9-zynq7000

TARGETS += $(TARGETS_ARMCORTEXA)
ifneq (,$(filter $(TARGETS_ARMCORTEXA),$(TARGET_FAMILY)-$(TARGET_SUBFAMILY)))
  TARGET_SUFF ?= armv7a
endif

# IA32
TARGETS_IA32 := ia32-generic

TARGETS += $(TARGETS_IA32)
ifneq (,$(filter $(TARGETS_IA32),$(TARGET_FAMILY)-$(TARGET_SUBFAMILY)))
  TARGET_SUFF ?= ia32
endif

#RISCV64
TARGETS_RISCV64 := \
	riscv64-spike \
	riscv64-virt

TARGETS += $(TARGETS_RISCV64)
ifneq (,$(filter $(TARGETS_RISCV64),$(TARGET_FAMILY)-$(TARGET_SUBFAMILY)))
  TARGET_SUFF ?= riscv64
endif

TARGETS += host-pc
ifeq ($(TARGET_FAMILY), host)
  TARGET_SUFF ?= host
endif

# Check target
ifeq (,$(filter $(TARGETS),$(TARGET_FAMILY)-$(TARGET_SUBFAMILY)))
  $(error Incorrect TARGET $(TARGET_FAMILY)-$(TARGET_SUBFAMILY). Available targets: $(TARGETS))
endif

# TODO: uncomment when all submodules would be moved to the new build system
#include $(MAKES_PATH)/../Makefile.$(TARGET_SUFF)
