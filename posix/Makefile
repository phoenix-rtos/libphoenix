#
# Makefile for libphoenix (posix compatibility)
#
# Copyright 2018 Phoenix Systems
# Author: Pawel Pisarczyk
#
ifeq (,$(TOPDIR))
$(error Only main Makefile can be used for compilation)
endif

SRCS := stubs.c utils.c idtree.c
OBJS := $(SRCS:.c=.o)

all: $(ARCH)

$(ARCH): $(OBJS)

# include after all dependencies are set
include $(TOPDIR)/Makefile.rules
