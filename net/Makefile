#
# Makefile for libphoenix (net)
#
# Copyright 2017 Phoenix Systems
# Author: Marek Białowąs
#
ifeq (,$(SRCDIR))
$(error Only main Makefile can be used for compilation)
endif

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)


all: check $(ARCH)

$(ARCH): $(OBJS)

# include after all dependencies are set
include $(SRCDIR)/Makefile.rules
