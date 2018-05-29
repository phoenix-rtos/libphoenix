#
# Makefile for libphoenix (arch/arm-imx)
#
# Copyright 2017 Phoenix Systems
# Author: Pawel Pisarczyk
#
ifeq (,$(SRCDIR))
$(error Only main Makefile can be used for compilation)
endif

ASMS := _start.S syscalls.S
SRCS := string.c jmp.c
OBJS := $(SRCS:.c=.o) $(ASMS:.S=.o)

-include .depend

all: check $(ARCH)

$(ARCH): $(OBJS)

# include after all dependencies are set
include $(SRCDIR)/Makefile.rules
