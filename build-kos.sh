#!/bin/bash
set -e

SDK_EDITION=Community-Edition
SDK_VERSION=1.1.1.13
TARGET_ARCH=aarch64
TARGET_OS=kos

NPROC=32

SDK_PATH=/opt/KasperskyOS-${SDK_EDITION}-${SDK_VERSION}
SYSROOT_PATH=${SDK_PATH}/sysroot-${TARGET_ARCH}-${TARGET_OS}

CC=${TARGET_ARCH}-${TARGET_OS}-gcc
CXX=${TARGET_ARCH}-${TARGET_OS}-g++

python3 buildscripts/scons.py \
    CC="${SDK_PATH}/toolchain/bin/${CC}" \
    CXX="${SDK_PATH}/toolchain/bin/${CXX}" \
    CCFLAGS="--sysroot=${SYSROOT_PATH}" \
    CXXFLAGS="--sysroot=${SYSROOT_PATH}" \
    TARGET_OS="${TARGET_OS}" \
    install-mongod -j${NPROC}

