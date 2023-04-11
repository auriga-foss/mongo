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

.PHONY: dump-vars
dump-vars:
	@echo "Variables:"
	@echo "\t BUILD_ROOT = $(BUILD_ROOT)"
	@echo "\t BUILD = $(BUILD)"
	@echo "\t SDK_EDITION = $(SDK_EDITION)"
	@echo "\t SDK_VERSION = $(SDK_VERSION)"
	@echo "\t SDK_PREFIX = $(SDK_PREFIX)"
	@echo "\t PATH = $(PATH)"
	@echo "\t TARGET = $(TARGET)"
	@echo "\t MONGOD_PORT = $(MONGOD_PORT)"
	@echo "\t MONGO_SRC = $(MONGO_SRC)"
	@echo "\t MONGO_BUILD_PATH = $(MONGO_BUILD_PATH)"

.PHONY: prebuild qemubuild hwbuild qemurun qemurun/fast
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

qemurun:
	$(MAKE) -C $(BUILD) sim

qemurun/fast:
	$(MAKE) -C $(BUILD) sim/fast

.PHONY: qemurun/gdb qemurun/gdb/fast gdb gdb/fast
qemurun/gdb:
	$(MAKE) -C $(BUILD) gdbsim

qemurun/gdb/fast:
	$(MAKE) -C $(BUILD) gdbsim/fast

# TODO: make QEMU GDB port configurable
export GDBOPTIONS = \
	-ex "target remote 127.0.0.1:1234" \
	--symbols=$(MONGO_BUILD_PATH)/install/bin/mongod.debug

gdb:
	$(MAKE) -C $(BUILD) gdb

gdb/fast:
	$(MAKE) -C $(BUILD) gdb/fast

.PHONY: clean clean-kos clean-mongo clean-all
clean: clean-kos
clean-kos:
	rm -rf $(BUILD)

clean-mongo:
	rm -rf $(MONGO_BUILD_PATH)

clean-all: clean clean-mongo

