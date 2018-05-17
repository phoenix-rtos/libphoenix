#
# Makefile for libphoenix (stdlib)
#
# Copyright 2017 Phoenix Systems
# Author: Pawel Pisarczyk
#
ifeq (,$(SRCDIR))
$(error Only main Makefile can be used for compilation)
endif

SRCS = strtoul.c strtoull.c malloc_dl.c rand.c bsearch.c exit.c qsort.c env.c
OBJS = $(SRCS:.c=.o)


all: check $(ARCH)

$(ARCH): $(OBJS)

# include after all dependencies are set
include $(SRCDIR)/Makefile.rules
