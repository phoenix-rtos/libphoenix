#!/bin/bash
#
# Shell script for building Phoenix-RTOS firmware
#
# Builder for Phoenix-RTOS core components
#
# Copyright 2018, 2019 Phoenix Systems
# Author: Kaja Swat, Aleksander Kaminski, Pawel Pisarczyk
#

b_log "Building phoenix-rtos-kernel"
KERNEL_MAKECMDGOALS="install-headers"
(cd phoenix-rtos-kernel/src/ && make $MAKEFLAGS $CLEAN $KERNEL_MAKECMDGOALS all)
cp -a phoenix-rtos-kernel/phoenix-$TARGET.elf _build
cp -a phoenix-rtos-kernel/phoenix-$TARGET.img _build

b_log "Building libphoenix"
(cd libphoenix && make $MAKEFLAGS $CLEAN all install)

b_log "Building phoenix-rtos-filesystems"
(cd phoenix-rtos-filesystems && make $MAKEFLAGS $CLEAN all)
b_install "$PREFIX_PROG_STRIPPED/meterfs" _build

b_log "Building phoenix-rtos-devices"
(cd phoenix-rtos-devices && make $MAKEFLAGS $CLEAN all)
b_install "$PREFIX_PROG_STRIPPED/stm32-multi" _build
