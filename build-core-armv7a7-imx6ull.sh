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
make -C "phoenix-rtos-kernel" $KERNEL_MAKECMDGOALS all

b_log "Building libphoenix"
make -C "libphoenix" all install

b_log "Building phoenix-rtos-filesystems"
make -C "phoenix-rtos-filesystems" all
b_install "$PREFIX_PROG_STRIPPED/dummyfs" /sbin

b_log "Building phoenix-rtos-devices"
make -C "phoenix-rtos-devices" all
b_install "$PREFIX_PROG_STRIPPED/imx6ull-gpio" /sbin
b_install "$PREFIX_PROG_STRIPPED/imx6ull-uart" /sbin
b_install "$PREFIX_PROG_STRIPPED/imx6ull-sdma" /sbin
b_install "$PREFIX_PROG_STRIPPED/imx6ull-nandtool" /sbin
b_install "$PREFIX_PROG_STRIPPED/imx6ull-flash" /sbin
b_install "$PREFIX_PROG_STRIPPED/imx6ull-otp" /sbin

b_log "Building phoenix-rtos-usb"
make -C "phoenix-rtos-usb" all
b_install "$PREFIX_PROG_STRIPPED/hostsrv" /sbin

b_log "Building coreutils"
make -C "phoenix-rtos-utils" all
b_install "$PREFIX_PROG_STRIPPED/psh" /bin
b_install "$PREFIX_PROG_STRIPPED/psd" /sbin
b_install "$PREFIX_PROG_STRIPPED/psd-old" /sbin

b_log "phoenix-rtos-lwip"
make -C "phoenix-rtos-lwip" all
b_install "$PREFIX_PROG_STRIPPED/lwip" /sbin

b_log "Building posixsrv"
make -C "phoenix-rtos-posixsrv" all
b_install "$PREFIX_PROG_STRIPPED/posixsrv" /bin

# FIXME: compile host tools using host-pc target?
b_log "Building hostutils"
make -C "phoenix-rtos-hostutils" -f Makefile.old $CLEAN all
cp "$PREFIX_BUILD_HOST/prog.stripped/phoenixd" "$PREFIX_BOOT"
cp "$PREFIX_BUILD_HOST/prog.stripped/psu" "$PREFIX_BOOT"

b_log "Building phoenix-rtos-corelibs"
make -C "phoenix-rtos-corelibs" all
