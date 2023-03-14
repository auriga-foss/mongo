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

# TODO:
# 1) Switch the build to dynamic linking when supported by the KOS CE SDK;
# 2) Enable support for HTTP client & Free Monitoring when libcurl is available
#    in the KOS CE SDK;
# 3) Enable wiredtiger;

python3 buildscripts/scons.py \
    --link-model=static \
    --enable-http-client=off \
    --enable-free-mon=off \
    --wiredtiger=off \
    CC="${SDK_PATH}/toolchain/bin/${CC}" \
    CXX="${SDK_PATH}/toolchain/bin/${CXX}" \
    CCFLAGS="--sysroot=${SYSROOT_PATH}" \
    CXXFLAGS="--sysroot=${SYSROOT_PATH}" \
    TARGET_OS="${TARGET_OS}" \
    install-mongod -j${NPROC}

