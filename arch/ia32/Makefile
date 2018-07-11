#
# Makefile for libphoenix (arch/ia32)
#
# Copyright 2017 Phoenix Systems
# Author: Pawel Pisarczyk
#
ifeq (,$(TOPDIR))
$(error Only main Makefile can be used for compilation)
endif

ASMS := syscalls.S _start.S jmp.S
SRCS := string.c
OBJS := $(SRCS:.c=.o) $(ASMS:.S=.o)

all: $(ARCH)

$(ARCH): $(OBJS)

# include after all dependencies are set
include $(TOPDIR)/Makefile.rules
