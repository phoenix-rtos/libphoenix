#
# Makefile for libphoenix
#
# Copyright 2017 Phoenix Systems
#
# %LICENSE%
#

SIL ?= @
MAKEFLAGS += --no-print-directory --output-sync

#TARGET ?= arm-imx
#TARGET ?= armv7-stm32-tiramisu
TARGET ?= ia32-qemu

VERSION = 0.2
SRCDIR := $(CURDIR)

SUBSYSTEMS := math stdio stdlib string sys ctype time fcntl unistd errno signal termios posix err locale
EXTRA_HEADER_DIRS := net netinet arpa

LIB := libphoenix.a

############ IA32 ############

ifneq (, $(findstring ia32, $(TARGET)))
	CROSS ?= i386-pc-phoenix-
	SUBDIRS = arch/ia32  $(SUBSYSTEMS)

	MKDEP = $(CROSS)gcc -MM
	MKDEPFLAGS = $(CFLAGS)

	CC = $(CROSS)gcc

	CFLAGS += -O2 -g -Wall -Wstrict-prototypes -I$(SRCDIR) -nostartfiles -nostdlib\
		-m32 -mtune=generic -mno-mmx -mno-sse -fno-pic -fno-pie\
		-fomit-frame-pointer -fno-strength-reduce -ffreestanding

	AR = $(CROSS)ar
	ARFLAGS = -r

	LD = $(CROSS)ld
	LDFLAGS = -m elf_i386
	GCCLIB := $(shell $(CC) $(CFLAGS) -print-libgcc-file-name)

	OBJCOPY = $(CROSS)objcopy
	OBJDUMP = $(CROSS)objdump

	STRIP = $(CROSS)strip
endif

############ ARMv7 (Cortex-M3/M4) ############

ifneq (, $(findstring armv7, $(TARGET)))
	CROSS ?= arm-phoenix-
	SUBDIRS = arch/armv7 $(SUBSYSTEMS)

	MKDEP = $(CROSS)gcc -MM
	MKDEPFLAGS = $(CFLAGS)

	CC = $(CROSS)gcc

	CFLAGS += -O2 -Wall -Wstrict-prototypes -g -I$(SRCDIR) -nostartfiles -nostdlib\
		-mcpu=cortex-m3 -mthumb \
		-fomit-frame-pointer -ffreestanding\
		-fpic -fpie -msingle-pic-base -mno-pic-data-is-text-relative\
		-DNOMMU

	CFLAGS += -fdata-sections -ffunction-sections

	AR = $(CROSS)ar
	ARFLAGS = -r

	LD = $(CROSS)ld
	LDFLAGS = -nostdlib -e _start --section-start .text=0 -Tbss=20000000 -z max-page-size=0x10
	LDFLAGS += --gc-sections
	GCCLIB := $(shell $(CC) $(CFLAGS) -print-libgcc-file-name)

	OBJCOPY = $(CROSS)objcopy
	OBJDUMP = $(CROSS)objdump

	STRIP = $(CROSS)strip
endif

############ ARM (Cortex-A5/A7/A9) ############

ifneq (, $(findstring arm-imx, $(TARGET)))
	CROSS ?= arm-phoenix-
	SUBDIRS = arch/arm-imx $(SUBSYSTEMS)

	MKDEP = $(CROSS)gcc -MM
	MKDEPFLAGS = $(CFLAGS)

	CC = $(CROSS)gcc

	CFLAGS += -Os -Wall -Wstrict-prototypes -g -I$(SRCDIR) -nostartfiles -nostdlib\
		-mcpu=cortex-a7 -mtune=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=hard -mthumb\
		-fomit-frame-pointer -ffreestanding -mno-unaligned-access

	CFLAGS += -fdata-sections -ffunction-sections

	AR = $(CROSS)ar
	ARFLAGS = -r

	LD = $(CROSS)ld
	LDFLAGS = -nostdlib -z max-page-size=0x1000
	LDFLAGS += --gc-sections
	GCCLIB := $(shell $(CC) $(CFLAGS) -print-libgcc-file-name)

	OBJCOPY = $(CROSS)objcopy
	OBJDUMP = $(CROSS)objdump
	STRIP = $(CROSS)strip
endif


CFLAGS += -DVERSION=\"$(VERSION)\"

ARCH = code.a
ARCHS := $(shell for i in $(SUBDIRS); do echo "$$i/$(ARCH)"; done)
OBJS := _startc.o
HEADERS := $(shell find . $(EXTRA_HEADER_DIRS) $(SUBDIRS) -maxdepth 1  -name \*.h)

SYSROOT 		:= $(shell $(CC) $(CFLAGS) -print-sysroot)
MULTILIB_DIR 	:= $(shell $(CC) $(CFLAGS) -print-multi-directory)
LIBC_INSTALL_DIR := $(SYSROOT)/lib/$(MULTILIB_DIR)
LIBC_INSTALL_NAMES := libc.a libm.a crt0.o libg.a
HEADERS_INSTALL_DIR := $(SYSROOT)/usr/include/
ifeq (/,$(SYSROOT))
$(error Sysroot is not supported by Your toolchain. Use cross-toolchain to compile)
endif

export SRCDIR SIL TARGET LIB CC CFLAGS MKDEP MKDEPFLAGS AR ARFLAGS LD LDFLAGS GCCLIB ARCH OBJDUMP STRIP HEADERS_INSTALL_DIR


all: subsystems $(OBJS) $(LIB) tests posixsrv


.c.o:
	@(printf "CC  %-24s  " "$<"; $(CC) -c $(CFLAGS) $<)

	@(file="$@"; \
	datasz=0;\
	textsz=0;\
	for i in `$(OBJDUMP) -t $$file | grep -e " O " | grep -v "\.rodata" | awk '{ print $$4 }'`; do \
		datasz=`echo $$(($$datasz + 0x$$i))`;\
	done; \
	for i in `$(OBJDUMP) -t $$file | grep -e "F" | awk '{ print $$5 }'`; do \
		textsz=`echo $$(($$textsz + 0x$$i))`;\
	done;\
	echo "data=$$datasz\t text=$$textsz")

$(OBJS): $(filter clean,$(MAKECMDGOALS))

subsystems: $(ARCHS)

%/$(ARCH): .FORCE $(filter clean,$(MAKECMDGOALS))
	@+echo "\033[1;32mCOMPILE $(@D)\033[0m";\
	if ! $(MAKE) -C "$(@D)"; then\
		exit 1;\
	fi;\

.FORCE:


$(LIB): $(ARCHS) $(OBJS)
	@echo "\033[1;34mLD $@\033[0m"

	$(AR) $(ARFLAGS) -o $(LIB) $(OBJS) $(shell for i in $(SUBDIRS); do k=`echo $$i | sed 's/\//\\\\\//g'`; $(AR) -t $$i/$(ARCH) | sed "s/^/$$k\//"; done;)

	@(echo "";\
	echo "=> libphoenix for [$(TARGET)] has been created";\
	echo "")


tests: $(LIB)
	@d=`pwd`;\
	echo "\033[1;32mCOMPILE test\033[0m";\
	if ! cd test; then\
		exit 1;\
	fi;\
	if ! $(MAKE); then\
		exit 1;\
	fi;\
	cd $$d;\


posixsrv: $(LIB)
	@d=`pwd`;\
	echo "\033[1;32mCOMPILE posixsrv\033[0m";\
	if ! cd posixsrv; then\
		exit 1;\
	fi;\
	if ! $(MAKE); then\
		exit 1;\
	fi;\
	cd $$d;\


depend:
	@for i in $(SUBDIRS) test; do\
		d=`pwd`;\
		echo "DEPEND $$i";\
		if ! cd $$i; then\
			exit 1;\
		fi;\
		if ! $(MAKE) -s depend; then\
			exit 1;\
		fi;\
		cd $$d;\
	done;

install: $(LIB)
	@echo "Installing into: $(LIBC_INSTALL_DIR)"; \
	mkdir -p "$(LIBC_INSTALL_DIR)" "$(HEADERS_INSTALL_DIR)"; \
	cp -a "$<" "$(LIBC_INSTALL_DIR)"; \
	for lib in $(LIBC_INSTALL_NAMES); do \
		ln -sf "$(LIBC_INSTALL_DIR)/$<" "$(LIBC_INSTALL_DIR)/$$lib"; \
	done; \
	for file in $(HEADERS); do\
		install -m 644 -D $${file} $(HEADERS_INSTALL_DIR)/$${file};\
	done

uninstall:
	rm -rf "$(LIBC_INSTALL_DIR)/$(LIB)"
	@for lib in $(LIBC_INSTALL_NAMES); do \
		rm -rf "$(LIBC_INSTALL_DIR)/$$lib"; \
	done
	@for file in $(HEADERS); do \
		rm -rf "$(HEADERS_INSTALL_DIR)/$${file}"; \
	done

clean:
	@rm -f core *.o $(LIB)
	@for i in $(SUBDIRS) test posixsrv; do\
		d=`pwd`;\
		echo "CLEAN $$i";\
		if ! cd $$i; then\
			exit 1;\
		fi;\
		if ! $(MAKE) clean; then\
			exit 1;\
		fi;\
		cd $$d;\
	done;


.PHONY: clean install uninstall
# DO NOT DELETE
