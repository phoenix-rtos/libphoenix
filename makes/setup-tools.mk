# Makefile for Phoenix-RTOS 3
#
# Copyright 2021 Phoenix Systems
#

# setting up necessary tool

#
# Establish sed tool
#
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
  SED := gsed
else
  SED := sed
endif

