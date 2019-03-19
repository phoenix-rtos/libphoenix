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

KERNEL_MAKECMDGOALS=""
if [ "X$CLEAN" == "Xclean" ]; then
	KERNEL_MAKECMDGOALS="$CLEAN install-headers"
fi

(cd phoenix-rtos-kernel/src/ && make $MAKEFLAGS $KERNEL_MAKECMDGOALS all)
cp -a phoenix-rtos-kernel/phoenix-arm-imx6ull.elf _build
cp -a phoenix-rtos-kernel/phoenix-arm-imx6ull.img _build

b_log "Building libphoenix"
(cd libphoenix && make $MAKEFLAGS $CLEAN all install)

b_log "Building psh"
(cd phoenix-rtos-coreutils && make $MAKEFLAGS $CLEAN b_psh)
b_install $PREFIX_PROG_STRIPPED/psh /bin

b_log "Building phoenix-rtos-filesystems"
(cd phoenix-rtos-filesystems && make $MAKEFLAGS $CLEAN all)
b_install "$PREFIX_PROG_STRIPPED/dummyfs" /sbin

b_log "Building phoenix-rtos-devices"
(cd phoenix-rtos-devices && make $MAKEFLAGS $CLEAN all)
b_install "$PREFIX_PROG_STRIPPED/imx6ull-gpio" /sbin
b_install "$PREFIX_PROG_STRIPPED/imx6ull-uart" /sbin
b_install "$PREFIX_PROG_STRIPPED/imx6ull-sdma" /sbin
b_install "$PREFIX_PROG_STRIPPED/imx6ull-nandtool" /sbin
b_install "$PREFIX_PROG_STRIPPED/imx6ull-flash" /sbin

b_log "Building psd"
(cd phoenix-rtos-coreutils && make $MAKEFLAGS b_psd)
b_install $PREFIX_PROG_STRIPPED/psd /sbin

b_log "Building phoenix-rtos-usb"
(cd phoenix-rtos-usb && make $MAKEFLAGS $CLEAN all)
b_install "$PREFIX_PROG_STRIPPED/usb" /sbin

b_log "phoenix-rtos-lwip"
(cd phoenix-rtos-lwip && make $MAKEFLAGS $CLEAN all)
b_install "$PREFIX_PROG_STRIPPED/lwip" /sbin

b_log "Building posixsrv"
(cd phoenix-rtos-posixsrv && make $MAKEFLAGS $CLEAN all)
b_install "$PREFIX_PROG_STRIPPED/posixsrv" /bin

b_log "Building phoenixd"
(cd phoenixd/ && make $MAKEFLAGS $CLEAN all)
cp phoenixd/phoenixd $PREFIX_BOOT
