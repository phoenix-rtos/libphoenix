#
# Makefile for libphoenix (signal)
#
# Copyright 2018 Phoenix Systems
# Author: Pawel Pisarczyk
#
ifeq (,$(TOPDIR))
$(error Only main Makefile can be used for compilation)
endif

SRCS = signal.c
OBJS := $(patsubst $(TOPDIR)/%,$(BUILD_DIR)/%,$(abspath $(SRCS:.c=.o)))

all: $(ARCH)

$(ARCH): $(OBJS)

# include after all dependencies are set
include $(TOPDIR)/Makefile.rules
