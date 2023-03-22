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

# For aarch64 target architecture, the SConstruct script sets the -march
# compiler option to armv8.2-a. KOS qemu does not support armv8.2-a. This
# causes some CPU instructions to fail.
# The most supported is armv8-a. Back to armv8-a.
MARCH=armv8-a

# TODO:
# 1) Switch the build to dynamic linking when supported by the KOS CE SDK;
# 2) Enable support for HTTP client & Free Monitoring when libcurl is available
#    in the KOS CE SDK;
# 3) Enable mozjs JS engine;

python3 buildscripts/scons.py \
    --link-model=static \
    --enable-http-client=off \
    --enable-free-mon=off \
    --wiredtiger=on \
    --js-engine=none \
    --use-system-boost \
    CC="${SDK_PATH}/toolchain/bin/${CC}" \
    CXX="${SDK_PATH}/toolchain/bin/${CXX}" \
    CCFLAGS="--sysroot=${SYSROOT_PATH} -march=${MARCH}" \
    CXXFLAGS="--sysroot=${SYSROOT_PATH}" \
    TARGET_OS="${TARGET_OS}" \
    install-mongod -j${NPROC}

