#
# Makefile for libphoenix - Phoenix-RTOS 3.0 kernel-userland interface
#
# Copyright 2017 Phoenix Systems
#
# %LICENSE%
#

#TARGET = armv7-stm32-tiramisu
TARGET ?= ia32-qemu
BIN := libphoenix.a
CC := gcc

ifneq (, $(findstring ia32, $(TARGET)))
  CROSS ?= i386-pc-phoenix-
  TARGET_SPECIFIC_DIR := ia32
else
  CROSS ?= arm-phoenix-
  TARGET_SPECIFIC_DIR := armv7
endif

ifeq ($(DEBUG), 1)
  CFLAGS := -O0 -ggdb3
else
  CFLAGS := -O3 -DNDEBUG
endif


CC := $(CROSS)$(CC)
CFLAGS := -Wall \
  -nostartfiles -nostdlib -fomit-frame-pointer -fno-strength-reduce -ffreestanding

AR := $(CROSS)$(AR)

SRCS := _start.c process.c printf.c memcpy.c memset.c strcmp.c strcpy.c strlen.c \
  strtol.c strtoul.c msg.c mutex.c thread.c time.c malloc.c mmap.c list.c
ASMS := $(wildcard $(TARGET_SPECIFIC_DIR)/*.S)

OBJS := $(ASMS:.S=.o) $(SRCS:.c=.o)


all: $(BIN)

$(BIN): $(BIN)($(OBJS))

%.d: %.c
	@set -e; rm -f $@; \
	$(CC) -M $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

clean:
	rm -f $(OBJS) $(BIN) $(SRCS:.c=.d) $(addsuffix .*, $(SRCS:.c=.d))

.PHONY: clean all
# required by ar
.NOTPARALLEL:

-include $(SRCS:.c=.d)
