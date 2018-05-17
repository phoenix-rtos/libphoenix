#
# Makefile for libphoenix (errno)
#
# Copyright 2018 Phoenix Systems
# Author: Pawel Pisarczyk
#

SRCS = errno.c
OBJS = $(SRCS:.c=.o)


all: check $(ARCH)

$(ARCH): $(OBJS)

# include after all dependencies are set
include $(SRCDIR)/Makefile.rules
