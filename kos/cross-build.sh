#!/bin/bash

SCRIPT_DIR="$(dirname "$(realpath "${0}")")"
BUILD=${1:-${SCRIPT_DIR}/build}

export SDK_EDITION=Community-Edition
export SDK_VERSION=1.1.1.13
export TARGET_ARCH=aarch64
export TARGET_OS=kos
export TARGET=${TARGET_ARCH}-${TARGET_OS}

export SDK_PREFIX=/opt/KasperskyOS-${SDK_EDITION}-${SDK_VERSION}
export SYSROOT_PATH=${SDK_PREFIX}/sysroot-${TARGET_ARCH}-${TARGET_OS}
export PATH="$SDK_PREFIX/toolchain/bin:$PATH"

export INSTALL_PREFIX="$BUILD/../install"

export LANG=C
export PKG_CONFIG=""

export BUILD_WITH_CLANG=
export BUILD_WITH_GCC=

TOOLCHAIN_SUFFIX=""
MONGOD_PORT=27018

if [ "$BUILD_WITH_CLANG" == "y" ];then
    TOOLCHAIN_SUFFIX="-clang"
fi

if [ "$BUILD_WITH_GCC" == "y" ];then
    TOOLCHAIN_SUFFIX="-gcc"
fi

"$SDK_PREFIX/toolchain/bin/cmake" -G "Unix Makefiles" -B "$BUILD" \
      -D CMAKE_BUILD_TYPE:STRING=Debug \
      -D CMAKE_INSTALL_PREFIX:STRING="$INSTALL_PREFIX" \
      -D CMAKE_FIND_ROOT_PATH="$([[ -f "$SCRIPT_DIR/additional_cmake_find_root.txt" ]] && cat "$SCRIPT_DIR/additional_cmake_find_root.txt")$PREFIX_DIR/sysroot-$TARGET" \
      -D CMAKE_TOOLCHAIN_FILE="$SDK_PREFIX/toolchain/share/toolchain-$TARGET$TOOLCHAIN_SUFFIX.cmake" \
      -D MONGOD_PORT=$MONGOD_PORT \
      "$SCRIPT_DIR/" && "$SDK_PREFIX/toolchain/bin/cmake" --build "$BUILD" --target sim
