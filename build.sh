#!/bin/bash
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
PREFIX_BUILD_HOST="$(pwd)/_build/host"
PREFIX_FS=$(realpath "_fs")
PREFIX_BOOT=$(realpath "_boot")

PREFIX_PROG="$PREFIX_BUILD/prog/"
PREFIX_PROG_STRIPPED="$PREFIX_BUILD/prog.stripped/"

PREFIX_A="$PREFIX_BUILD/lib/"
PREFIX_H="$PREFIX_BUILD/include/"

PREFIX_ROOTFS="$PREFIX_FS/root/"

CFLAGS="${CFLAGS} -I${PREFIX_H}"
LDFLAGS="$LDFLAGS -L$PREFIX_A"
CC=${CROSS}gcc
AS=${CROSS}as
LD=${CROSS}ld
AR=${CROSS}ar

MAKEFLAGS="--no-print-directory -j 19"

export TARGET TARGET_FAMILY TOPDIR PREFIX_BUILD PREFIX_BUILD_HOST\
	PREFIX_FS PREFIX_BOOT PREFIX_PROG PREFIX_PROG_STRIPPED PREFIX_A\
	PREFIX_H PREFIX_ROOTFS CROSS CFLAGS LDFLAGS CC LD AR CLEAN MAKEFLAGS

. ./phoenix-rtos-build/build.subr

#
# Parse command line
#
if [ $# -lt 1 ]; then
	echo "Build options should be specified!"
	echo "Usage: build.sh [clean] [all] [fs] [core] [ports] [project] [image]";
	exit -1;
fi

B_FS="n"
B_CORE="n"
B_PORTS="n"
B_PROJECT="n"
B_IMAGE="n"
B_UPDATE_PKG="n"

for i in $*; do
	case "$i"
	in
		clean)
			CLEAN="clean"
			shift;;
		fs)
			B_FS="y"
			shift;;
		core)
			B_CORE="y"
			shift;;
		ports)
			B_PORTS="y"
			shift;;
		project)
			B_PROJECT="y"
			shift;;
		image)
			B_IMAGE="y"
			shift;;
		update_pkg)
			B_UPDATE_PKG="y"
			shift;;
		all)
			B_FS="y"; B_CORE="y"; B_PORTS="y"; B_PROJECT="y"; B_IMAGE="y"; B_UPDATE_PKG="y";
			shift;;
	esac;
done

export BUSYBOX_CONFIG=""

#
# Preparing filesystem
#
if [ "X${B_FS}" == "Xy" ]; then
	b_log "Preparing filesystem"
	if [ "X$CLEAN" == "Xclean" ]; then
		rm -fr $PREFIX_FS/root/*
	fi

	mkdir -p $PREFIX_FS/root
	cp -a $PREFIX_FS/root-skel/. $PREFIX_FS/root/

	b_log "Saving git-version"
	echo " "$(git rev-parse HEAD)" incotex" > $PREFIX_FS/root/etc/git-version
	git submodule status >> $PREFIX_FS/root/etc/git-version
fi

mkdir -p $PREFIX_BUILD
mkdir -p $PREFIX_BUILD_HOST

#
# Build core part
#
if [ "X${B_CORE}" == "Xy" ]; then
	./phoenix-rtos-build/build-core-${TARGET_SUBFAMILY}.sh
fi

#
# Build ports
#
if [ "X${B_PORTS}" == "Xy" ]; then
	./phoenix-rtos-ports/build.sh
fi

#
# Build project part
#
if [ "X${B_PROJECT}" == "Xy" ]; then
	b_build
fi

#
# Build final filesystems
#
if [ "X${B_IMAGE}" == "Xy" ]; then
	b_image
fi

#
# Build final filesystems
#
if [ "X${B_UPDATE_PKG}" == "Xy" ]; then
	b_update_pkg
fi
