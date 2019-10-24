#!/bin/bash
#
# Shell script for building Phoenix-RTOS firmware
#
# Builder for Phoenix-RTOS core components
#
# Copyright 2019 Phoenix Systems
# Author: Kaja Swat, Aleksander Kaminski, Pawel Pisarczyk, Lukasz Kosinski
#

b_log "Building phoenix-rtos-kernel"

KERNEL_MAKECMDGOALS="install-headers"
if [ "X$CLEAN" == "Xclean" ]; then
	KERNEL_MAKECMDGOALS="$CLEAN $KERNEL_MAKECMDGOALS"
fi

(cd phoenix-rtos-kernel/src && make $MAKEFLAGS $KERNEL_MAKECMDGOALS all)
cp -a phoenix-rtos-kernel/phoenix-ia32-qemu.elf _build
cp -a phoenix-rtos-kernel/phoenix-ia32-qemu.img _build

b_log "Building libphoenix"
(cd libphoenix && make $MAKEFLAGS $CLEAN all install)

b_log "Building phoenix-rtos-filesystems"
(cd phoenix-rtos-filesystems && make $MAKEFLAGS $CLEAN all)
b_install "$PREFIX_PROG_STRIPPED/dummyfs" /sbin
b_install "$PREFIX_PROG_STRIPPED/ext2" /sbin

b_log "Building phoenix-rtos-devices"
(cd phoenix-rtos-devices && make $MAKEFLAGS $CLEAN all)
b_install "$PREFIX_PROG_STRIPPED/pc-tty" /sbin
b_install "$PREFIX_PROG_STRIPPED/pc-uart" /sbin
b_install "$PREFIX_PROG_STRIPPED/pc-ata" /sbin

b_log "Building psh"
(cd psh && make $MAKEFLAGS $CLEAN all)
b_install "$PREFIX_PROG_STRIPPED/psh" /bin
