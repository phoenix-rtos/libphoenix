#!/bin/bash
# $1 - toolchain target (e.g. arm-phoenix)
# $2 - toolchain install absolute path (i.e. no "." or ".." in the path)

set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

log() {
    echo -e "\e[35;1m$*\e[0m"
}

# targets for libphoenix and phoenix-rtos-kernel for installing headers
declare -A TOOLCHAN_TO_PHOENIX_TARGET=(
    [arm-phoenix]="armv7a7-imx6ull"
    [i386-pc-phoenix]="ia32-generic"
    [riscv64-phoenix]="riscv64-generic"
)

if [ -z "$1" ] || [ -z "${TOOLCHAN_TO_PHOENIX_TARGET[$1]}" ]; then
    echo "Missing or invalid target provided! Abort."
    echo "officially supported targets:"
    printf "%s\n" "${!TOOLCHAN_TO_PHOENIX_TARGET[@]}"
    exit 1
fi

PHOENIX_TARGET="${TOOLCHAN_TO_PHOENIX_TARGET[$1]}"

if [ -z "$2" ]; then
    echo "No toolchain install path provided! Abort."
    exit 1
fi

if [ "${2:0:1}" != "/" ]; then
    echo "Path must not be relative."
    exit 1
fi

if [ ! -f "$SCRIPT_DIR/../../phoenix-rtos-kernel/Makefile" ] || [ ! -f "$SCRIPT_DIR/../../libphoenix/Makefile" ]; then
    echo "phoenix-rtos-kernel or libphoenix missing! Please use full phoenix-rtos-project repo for toolchain building. Abort."
    exit 1
fi

# old legacy versions of the compiler:
#BINUTILS=binutils-2.28
#GCC=gcc-7.1.0

BINUTILS=binutils-2.34
GCC=gcc-9.3.0

TARGET="$1"
BUILD_ROOT="$2"
TOOLCHAIN_PREFIX="${BUILD_ROOT}/${TARGET}"
SYSROOT="${TOOLCHAIN_PREFIX}/${TARGET}"
MAKEFLAGS="-j9 -s"
export MAKEFLAGS

mkdir -p "${TOOLCHAIN_PREFIX}"
cp ./*.patch "${BUILD_ROOT}"
cd "${BUILD_ROOT}"

download() {
    log "downloading packages"

    [ ! -f ${BINUTILS}.tar.bz2 ] && wget "http://ftp.gnu.org/gnu/binutils/${BINUTILS}.tar.bz2"
    [ ! -f ${GCC}.tar.xz ] && wget "http://www.mirrorservice.org/sites/ftp.gnu.org/gnu/gcc/${GCC}/${GCC}.tar.xz"

    log "extracting packages"
    [ ! -d ${BINUTILS} ] && tar jxf ${BINUTILS}.tar.bz2
    [ ! -d ${GCC} ] && tar Jxf ${GCC}.tar.xz

    log "downloading GCC dependencies"
    (cd "$GCC" && ./contrib/download_prerequisites)
}

build_binutils() {
    log "patching ${BINUTILS}"
    for patchfile in "${BINUTILS}"-*.patch; do
        if [ ! -f "${BINUTILS}/$patchfile.applied" ]; then
            patch -d "${BINUTILS}" -p1 < "$patchfile"
            touch "${BINUTILS}/$patchfile.applied"
        fi
    done

    log "building binutils"
    rm -rf "${BINUTILS}/build"
    mkdir -p "${BINUTILS}/build"
    pushd "${BINUTILS}/build" > /dev/null

    ../configure --target="${TARGET}" --prefix="${TOOLCHAIN_PREFIX}" \
                 --with-sysroot="${SYSROOT}" --enable-lto
    make

    log "installing binutils"
    make install
    popd > /dev/null
}

build_gcc_stage1() {
    log "patching ${GCC}"
    for patchfile in "${GCC}"-*.patch; do
        if [ ! -f "${GCC}/$patchfile.applied" ]; then
            patch -d "${GCC}" -p1 < "$patchfile"
            touch "${GCC}/$patchfile.applied"
        fi
    done

    log "building GCC (stage1)"
    rm -rf "${GCC}/build"
    mkdir -p "${GCC}/build"
    pushd "${GCC}/build" > /dev/null

    # GCC compilation options
    # --with-sysroot -> cross-compiler sysroot
    # --with-newlib -> do note generate standard library includes by fixincludes, do not include _eprintf in libgcc
    # --disable-libssp -> stack smashing protector library disabled
    # --disable-nls -> all compiler messages will be in english
    # --disable-tls -> disable Thread Local Storage as it's not supported Yet (note: C11 incompatibility?)
    # --enable-initfini-array -> force init/fini array support instead of .init .fini sections
    # --disable-decimal-float -> not relevant for other than i386 and PowerPC
    # --disable-libquadmath -> not using fortran and quad floats


    # stage1 compiler (gcc only)
    ../configure --target="${TARGET}" --prefix="${TOOLCHAIN_PREFIX}" \
                 --with-sysroot="${SYSROOT}" \
                 --enable-languages=c,c++ --with-newlib \
                 --with-headers=yes \
                 --disable-tls \
                 --enable-initfini-array \
                 --disable-decimal-float \
                 --disable-libquadmath \
                 --disable-libssp --disable-nls

    make all-gcc

    log "installing GCC (stage1)"
    make install-gcc
    popd > /dev/null
}

build_libc() {
    # use new compiler for the below builds
    OLDPATH="$PATH"
    PATH="$TOOLCHAIN_PREFIX/bin":$PATH
    export PATH

    # standard library headers should be installed in $SYSROOT/include
    # for fixincludes to work the headers need to be in $SYSROOT/usr/include, for libgcc compilation in $SYSROOT/include
    # create symlink for this stage (arm-none-eabi-gcc does the same - see https://github.com/xpack-dev-tools/arm-gcc-original-scripts/blob/master/build-toolchain.sh)
    # FIXME: keep the symlink for now until install dir changes in libphoenix and kernel would be well-propagated
    ln -sf . "${SYSROOT}/usr"

    log "installing kernel headers"
    make -C "$SCRIPT_DIR/../../phoenix-rtos-kernel" TARGET="$PHOENIX_TARGET" install-headers

    # FIXME: libphoenix should be installed for all supported multilib target variants
    log "installing libphoenix"
    make -C "$SCRIPT_DIR/../../libphoenix" TARGET="$PHOENIX_TARGET" clean install

    PATH="$OLDPATH"
}

build_gcc_stage2() {
    pushd "$BUILD_ROOT/${GCC}/build" > /dev/null

    # (hackish) instead of reconfiguring and rebuilding whole gcc
    # just force rebuilding internal includes (and fixincludes)
    # remove stamp file for internal headers generation
    rm gcc/stmp-int-hdrs

    log "building GCC (stage2)"
    make all-gcc all-target-libgcc

    log "installing GCC (stage2)"
    make install-gcc install-target-libgcc

    # TODO: build libstdc++ -> for now it fails because of libphoenix missing features

    # FIXME: keep the symlink for now until install dir changes in libphoenix and kernel would be well-propagated
    #rm -r "${SYSROOT:?}/usr"
    popd > /dev/null
}

strip_binaries() {
    log "stripping binaries"
    if [ "$(uname)" = "Darwin" ]; then
        find "$TOOLCHAIN_PREFIX" -type f -perm +111 -exec strip {} + || true
    else
        find "$TOOLCHAIN_PREFIX" -type f -perm /111 -exec strip {} + || true
    fi

    # NOTE: we could also strip target libraries, but let's leave debug sections for ease of development
}


### MAIN ###

# comment out some parts if You need "incremental build" for testing

download;
build_binutils;
build_gcc_stage1;

build_libc;
build_gcc_stage2;

strip_binaries;

echo "Toolchain for target family '$TARGET' has been installed in '$TOOLCHAIN_PREFIX'"
echo "Please add it to PATH:"
echo "PATH=$TOOLCHAIN_PREFIX/bin:\$PATH"
