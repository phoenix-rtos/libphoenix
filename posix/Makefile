#
# Makefile for libphoenix (posix compatibility)
#
# Copyright 2018 Phoenix Systems
# Author: Pawel Pisarczyk
#
ifeq (,$(SRCDIR))
$(error Only main Makefile can be used for compilation)
endif

SRCS = stubs.c utils.c
OBJS = $(SRCS:.c=.o)


all: check $(ARCH)

$(ARCH): $(OBJS)

# include after all dependencies are set
include $(SRCDIR)/Makefile.rules
