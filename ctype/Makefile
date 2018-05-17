#
# Makefile for libphoenix (ctype)
#
# Copyright 2012-2015 Phoenix Systems
# Copyright 2001, 2005-2006 Pawel Pisarczyk
#
ifeq (,$(SRCDIR))
$(error Only main Makefile can be used for compilation)
endif

SRCS = ctype.c
OBJS = $(SRCS:.c=.o)

-include .depend

all: check $(ARCH)

$(ARCH): $(OBJS)

# include after all dependencies are set
include $(SRCDIR)/Makefile.rules
