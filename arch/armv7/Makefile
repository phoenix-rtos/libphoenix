#
# Makefile for libphoenix (arch/armv7)
#
# Copyright 2017 Phoenix Systems
# Author: Pawel Pisarczyk
#
ifeq (,$(SRCDIR))
$(error Only main Makefile can be used for compilation)
endif

ASMS := _start.S syscalls.S
SRCS := string.c
OBJS := $(ASMS:.S=.o) $(SRCS:.c=.o)

-include .depend

all: check $(ARCH)

$(ARCH): $(OBJS)

# include after all dependencies are set
include $(SRCDIR)/Makefile.rules
