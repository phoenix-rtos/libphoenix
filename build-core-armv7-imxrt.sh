#!/bin/bash
#
# Shell script for building Phoenix-RTOS firmware
#
# Builder for Phoenix-RTOS core components
#
# Copyright 2018, 2019 Phoenix Systems
# Author: Kaja Swat, Aleksander Kaminski, Pawel Pisarczyk, Lukasz Kosinski
#

b_log "Building phoenix-rtos-kernel"

KERNEL_MAKECMDGOALS="install-headers"
if [ "X$CLEAN" == "Xclean" ]; then
	KERNEL_MAKECMDGOALS="$CLEAN $KERNEL_MAKECMDGOALS"
fi

(cd phoenix-rtos-kernel/src/ && make $MAKEFLAGS $KERNEL_MAKECMDGOALS all)
cp -a "phoenix-rtos-kernel/phoenix-${TARGET}.elf" _build
cp -a "phoenix-rtos-kernel/phoenix-${TARGET}.img" _build

b_log "Building libphoenix"
(cd libphoenix && make $MAKEFLAGS $CLEAN all install)

b_log "Building phoenix-rtos-filesystems"
(cd phoenix-rtos-filesystems && make $MAKEFLAGS $CLEAN all)
cp "$PREFIX_PROG_STRIPPED/dummyfs" _build
cp "$PREFIX_PROG_STRIPPED/meterfs" _build

b_log "Building phoenix-rtos-devices"
(cd phoenix-rtos-devices && make $MAKEFLAGS $CLEAN all)
cp "$PREFIX_PROG_STRIPPED/imxrt-multi" _build
cp "$PREFIX_PROG_STRIPPED/imxrt-flash" _build

b_log "Building coreutils"
(cd phoenix-rtos-coreutils && make $MAKEFLAGS $CLEAN all)
cp "$PREFIX_PROG_STRIPPED/psh" _build
cp "$PREFIX_PROG_STRIPPED/psd" _build

b_log "Building hostutils"
(cd phoenix-rtos-hostutils/ && make $MAKEFLAGS $CLEAN all)
cp "$PREFIX_BUILD_HOST/prog.stripped/phoenixd" $PREFIX_BOOT
cp "$PREFIX_BUILD_HOST/prog.stripped/psu" $PREFIX_BOOT
