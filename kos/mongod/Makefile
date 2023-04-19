BUILD_ROOT = $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
BUILD = $(BUILD_ROOT)/build-$(TARGET_ARCH)
MONGO_SRC = $(abspath $(BUILD_ROOT)/../../)

ifneq ($(strip $(V)),1)
MAKEFLAGS += -s --no-print-directory
endif

export INSTALL_PREFIX = $(BUILD)/../install

export SDK_EDITION ?= Community-Edition
export SDK_VERSION ?= 1.1.1.13
export SDK_PREFIX  ?= /opt/KasperskyOS-${SDK_EDITION}-${SDK_VERSION}

export PATH := $(SDK_PREFIX)/toolchain/bin:$(PATH)

export TARGET_ARCH  ?= aarch64
export TARGET_OS    := kos
export TARGET       := ${TARGET_ARCH}-${TARGET_OS}
export SYSROOT_PATH := ${SDK_PREFIX}/sysroot-${TARGET_ARCH}-${TARGET_OS}

export CC  := ${TARGET_ARCH}-${TARGET_OS}-gcc
export CXX := ${TARGET_ARCH}-${TARGET_OS}-g++

export LANG = C
export TOOLCHAIN_SUFFIX ?= ""

# localhost port to be forwarded to QEMU guest
export MONGOD_PORT ?= 27018
export MONGO_BUILD_PATH ?= $(MONGO_SRC)/build/kos

# TODO: enable this logic in SConstruct for KOS
export DEBUG_ENABLED ?= y
DEBUG_CCFLAGS :=
ifeq ($(strip $(DEBUG_ENABLED)),y)
DEBUG_CCFLAGS += -ggdb -Og
endif

ifeq ($(strip $(TARGET_ARCH)),aarch64)
QEMU_OPTS = -m 2048 -machine vexpress-a15,secure=on -cpu cortex-a72 \
			-nographic -monitor none -smp 4 -serial stdio \
		    -nic user,hostfwd=tcp:127.0.0.1:$(MONGOD_PORT)-10.0.2.10:27017
else
$(error Bad target architecture: $(TARGET_ARCH). \
        No architectures other than aarch64 are currently supported)
endif

QEMU_SERIAL_PORT ?= 12345
QEMU_GDB_PORT ?= 1234
QEMU_GDB_OPTS = -serial tcp::$(QEMU_SERIAL_PORT),server,nowait \
				-S -gdb tcp::$(QEMU_GDB_PORT)

NPROC ?= 32

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
.PHONY: compile-mongod
compile-mongod:
	cd $(MONGO_SRC) && python3 buildscripts/scons.py \
		--link-model=static \
		--enable-http-client=off \
		--enable-free-mon=off \
		--wiredtiger=on \
		--js-engine=none \
		--use-system-boost \
		--build-dir=$(MONGO_BUILD_PATH) \
		CC="${SDK_PREFIX}/toolchain/bin/${CC}" \
		CXX="${SDK_PREFIX}/toolchain/bin/${CXX}" \
		CCFLAGS="--sysroot=${SYSROOT_PATH} -march=${MARCH} ${DEBUG_CCFLAGS}" \
		TARGET_OS="${TARGET_OS}" \
		install-mongod -j${NPROC}

# Note: used even if DEBUG_ENABLED=n to prevent scons from rebuilding the
# binary due to renaming
$(MONGO_BUILD_PATH)/install/bin/mongod.nodebug: $(MONGO_BUILD_PATH)/install/bin/mongod
	$(TARGET)-strip -g -o $@ $<

.PHONY: help dump-vars

help:
	@echo "The following targets are defined by this Makefile:"
	@echo "    help:             prints this message;"
	@echo "    dump-vars:        dump Makefile variables;"
	@echo "    compile-mongod:   compile and install mongod;"
	@echo "    prebuild:         generate build files (Makefiles) from"
	@echo "                      CMakeLists.txt scenario;"
	@echo "    qemubuild:        build KOS image to run on QEMU;"
	@echo "    hwbuild:          build KOS image to run on RaspberryPi;"
	@echo "    qemurun:          run using QEMU"
	@echo "                      (options are specified in QEMU_OPTS variable);"
	@echo "    qemurun/gdb:      run using QEMU. QEMU will be run with a GDB"
	@echo "                      server listening on tcp::QEMU_GDB_PORT"
	@echo "                      (GDB options are specified in QEMU_GDB_OPTS"
	@echo "                      variable);"
	@echo "    run/gdb:          run GDB, connect to server, add symbol file;"
	@echo "    clean:            aux. target (currently equal to clean-kos);"
	@echo "    clean-kos:        remove KOS build directory;"
	@echo "    clean-mongo:      remove MongoDB build directory;"
	@echo "    clean-all:        remove KOS & MongoDB build directories;"
	@echo ""
	@echo "The following targets are wrappers for those predefined by the KOS:"
	@echo "    sim:              rebuild image and run using QEMU;"
	@echo "    sim/fast:         'fast' version of the sim target. Does the"
	@echo "                      same, except it DOESN'T rebuild an image;"
	@echo "    gdbsim:           rebuild image, run using QEMU. QEMU will be"
	@echo "                      run with a running GDB server waiting for a"
	@echo "                      connection on tcp::1234;"
	@echo "    gdbsim/fast:      'fast' version of the gdbsim target. Does the"
	@echo "                      same, except it DOESN'T rebuild an image;"
	@echo "    gdb:              rebuild image and run GDB for target arch;"
	@echo "    gdb/fast:         'fast' version of the gdb target. Does the"
	@echo "                      same, except it DOESN'T rebuild an image;"


dump-vars:
	@echo "Variables:"
	@echo "    BUILD_ROOT = $(BUILD_ROOT)"
	@echo "    BUILD = $(BUILD)"
	@echo "    SDK_EDITION = $(SDK_EDITION)"
	@echo "    SDK_VERSION = $(SDK_VERSION)"
	@echo "    SDK_PREFIX = $(SDK_PREFIX)"
	@echo "    PATH = $(PATH)"
	@echo "    TARGET = $(TARGET)"
	@echo "    MONGOD_PORT = $(MONGOD_PORT)"
	@echo "    MONGO_SRC = $(MONGO_SRC)"
	@echo "    MONGO_BUILD_PATH = $(MONGO_BUILD_PATH)"

.PHONY: prebuild qemubuild hwbuild
prebuild: $(MONGO_BUILD_PATH)/install/bin/mongod.nodebug
	$(SDK_PREFIX)/toolchain/bin/cmake -G "Unix Makefiles" \
		-B $(BUILD) \
		-D CMAKE_BUILD_TYPE:STRING=Debug \
		-D CMAKE_INSTALL_PREFIX:STRING="$(INSTALL_PREFIX)" \
		-D CMAKE_TOOLCHAIN_FILE="$(SDK_PREFIX)/toolchain/share/toolchain-$(TARGET)$(TOOLCHAIN_SUFFIX).cmake" \
		-D MONGOD_PORT=$(MONGOD_PORT) \
		-D MONGOD_BINARY_PATH=$(MONGO_BUILD_PATH)/install/bin/mongod.nodebug \
		$(BUILD_ROOT)/

qemubuild: prebuild
	$(MAKE) -C $(BUILD) kos-qemu-image

hwbuild: prebuild
	$(MAKE) -C $(BUILD) kos-image

.PHONY: qemurun qemurun/gdb run/gdb
qemurun:
	qemu-system-$(TARGET_ARCH) $(QEMU_OPTS) \
		-kernel $(BUILD)/einit/kos-qemu-image

qemurun/gdb:
	qemu-system-$(TARGET_ARCH) $(QEMU_OPTS) $(QEMU_GDB_OPTS) \
		-kernel $(BUILD)/einit/kos-qemu-image

run/gdb:
	$(TARGET)-gdb -ex "target remote 127.0.0.1:$(QEMU_GDB_PORT)" \
		--symbols=$(MONGO_BUILD_PATH)/install/bin/mongod

# Note: These targets are wrappers for those predefined by the KOS build system
WRAPPER_TARGETS = sim sim/fast gdbsim gdbsim/fast gdb gdb/fast
.PHONY: $(WRAPPER_TARGETS)
$(WRAPPER_TARGETS): $(BUILD)/Makefile
	$(MAKE) -C $(<D) $@

.PHONY: clean clean-kos clean-mongo clean-all
clean: clean-kos
clean-kos:
	rm -rf $(BUILD)

clean-mongo:
	rm -rf $(MONGO_BUILD_PATH)

clean-all: clean clean-mongo

