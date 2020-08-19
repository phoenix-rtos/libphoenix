#!/bin/bash
#
# Shell script for building Phoenix-RTOS firmware
#
# Builder for Phoenix-RTOS core components
#
# Copyright 2018, 2019 Phoenix Systems
# Author: Kaja Swat, Aleksander Kaminski, Pawel Pisarczyk
#

# fail immediately if any of the commands fails
set -e

b_log "Building phoenix-rtos-kernel"
KERNEL_MAKECMDGOALS="install-headers"
(cd phoenix-rtos-kernel && make $MAKEFLAGS $CLEAN $KERNEL_MAKECMDGOALS all)

b_log "Building libphoenix"
(cd libphoenix && make $MAKEFLAGS $CLEAN all install)

b_log "Building phoenix-rtos-filesystems"
(cd phoenix-rtos-filesystems && make $MAKEFLAGS $CLEAN all)
b_install "$PREFIX_PROG_STRIPPED/dummyfs" /sbin

b_log "Building phoenix-rtos-devices"
(cd phoenix-rtos-devices && make $MAKEFLAGS $CLEAN all)
#b_install "$PREFIX_PROG_STRIPPED/imx6ull-gpio" /sbin

b_log "Building phoenix-rtos-usb"
(cd phoenix-rtos-usb && make $MAKEFLAGS $CLEAN all)

b_log "Building coreutils"
(cd phoenix-rtos-utils && make $MAKEFLAGS $CLEAN all)
b_install $PREFIX_PROG_STRIPPED/psh /bin
b_install $PREFIX_PROG_STRIPPED/psd /sbin

#b_log "phoenix-rtos-lwip"
#(cd phoenix-rtos-lwip && make $MAKEFLAGS $CLEAN all)
#b_install "$PREFIX_PROG_STRIPPED/lwip" /sbin

#b_log "Building posixsrv"
#(cd phoenix-rtos-posixsrv && make $MAKEFLAGS $CLEAN all)
#b_install "$PREFIX_PROG_STRIPPED/posixsrv" /bin

b_log "Building hostutils"
(cd phoenix-rtos-hostutils/ && make $MAKEFLAGS $CLEAN all)
mkdir -p "$PREFIX_BOOT"
cp "$PREFIX_BUILD_HOST/prog.stripped/phoenixd" "$PREFIX_BOOT"
cp "$PREFIX_BUILD_HOST/prog.stripped/psu" "$PREFIX_BOOT"

b_log "Building phoenix-rtos-corelibs"
(cd phoenix-rtos-corelibs/ && make $MAKEFLAGS $CLEAN all)
