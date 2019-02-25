#!/bin/sh
#
# Shell script for building Phoenix-RTOS based firmware
#
# Main builder
#
# Copyright 2018, 2019 Phoenix Systems
# Author: Kaja Swat, Aleksander Kaminski, Pawel Pisarczyk
#

set -e

. ./build.project

TOPDIR="$(pwd)"

PREFIX_BUILD="$(pwd)/_build/$TARGET"
PREFIX_FS=$(realpath "_fs")
PREFIX_BOOT=$(realpath "_boot")

PREFIX_PROG="$PREFIX_BUILD/prog/"
PREFIX_PROG_STRIPPED="$PREFIX_BUILD/prog.stripped/"

PREFIX_A="$PREFIX_BUILD/lib/"
PREFIX_H="$PREFIX_BUILD/include/"

CROSS=arm-phoenix-
CFLAGS="${CFLAGS} -I${PREFIX_H}"
LDFLAGS="$LDFLAGS -L$PREFIX_A"
CC=${CROSS}gcc
AS=${CROSS}as
LD=${CROSS}ld
AR=${CROSS}ar

MAKEFLAGS="--no-print-directory -j 19"

export TARGET TARGET_FAMILY TOPDIR PREFIX_BUILD PREFIX_FS PREFIX_BOOT PREFIX_PROG PREFIX_PROG_STRIPPED PREFIX_A PREFIX_H CROSS CFLAGS LDFLAGS CC LD AR CLEAN MAKEFLAGS

. ./phoenix-rtos-build/build.subr

export BUSYBOX_CONFIG=""

#
# Preparing filesystem
#
b_log "Preparing filesystem"
if [ "X$CLEAN" == "Xclean" ]; then
	rm -fr $PREFIX_FS/root/*
fi

mkdir -p $PREFIX_FS/root
cp -a $PREFIX_FS/root-skel/* $PREFIX_FS/root

b_log "Saving git-version"
echo " "$(git rev-parse HEAD)" incotex" > $PREFIX_FS/root/etc/git-version
git submodule status >> $PREFIX_FS/root/etc/git-version

#
# Build core part
#
mkdir -p $PREFIX_BUILD
./phoenix-rtos-build/build-core.sh

#
# Build ports
#
#./phoenix-rtos-ports/busybox/build.sh

#
# Build project part
#
b_build

#
# Build final filesystems
#
b_createfs
