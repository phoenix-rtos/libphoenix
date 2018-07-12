#
# Makefile for libphoenix (foreign)
#
# Copyright 2018 Phoenix Systems
# Author: Jan Sikorski
#
ifeq (,$(SRCDIR))
$(error Only main Makefile can be used for compilation)
endif

SRCS = collate.c collcmp.c fnmatch.c glob.c regcomp.c regerror.c regexec.c regfree.c #wordexp.c wordfree.c
OBJS = $(SRCS:.c=.o)


all: check $(ARCH)

$(ARCH): $(OBJS)

# include after all dependencies are set
include $(SRCDIR)/Makefile.rules
