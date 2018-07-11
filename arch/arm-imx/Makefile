#
# Makefile for libphoenix (arch/arm-imx)
#
# Copyright 2017 Phoenix Systems
# Author: Pawel Pisarczyk
#
ifeq (,$(TOPDIR))
$(error Only main Makefile can be used for compilation)
endif

ASMS := _start.S syscalls.S
SRCS := string.c jmp.c
OBJS := $(patsubst $(TOPDIR)/%,$(BUILD_DIR)/%,$(abspath $(SRCS:.c=.o))) $(patsubst $(TOPDIR)/%,$(BUILD_DIR)/%,$(abspath $(ASMS:.S=.o)))

all: $(ARCH)

$(ARCH): $(OBJS)

# include after all dependencies are set
include $(TOPDIR)/Makefile.rules
