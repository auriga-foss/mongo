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

export MONGO_BUILD_PATH ?= $(MONGO_SRC)/build/kos

IMG_DIR = $(BUILD)/sdcard
IMG_NAME = sdcard0.img
IMG = $(BUILD)/$(IMG_NAME)
IMG_FORMAT = fat32

IMG_TEST_DIR = /tests
TEST_SRC_DIR = $(MONGO_BUILD_PATH)/install/bin
TEST_DST_DIR = $(IMG_DIR)/$(IMG_TEST_DIR)

TESTS = $(patsubst $(TEST_SRC_DIR)%, $(TEST_DST_DIR)%, $(wildcard $(TEST_SRC_DIR)/*_test))

# CHOSEN_TESTS variable contains list of unit-tests to be run on KOS platform
# Notes:
# 1) SD card image contains all available Mongo unit-tests
# 2) Empty CHOSEN_TESTS causes running of all unit-tests 
CHOSEN_TESTS ?= db_bson_test \
               	db_sorter_test

# Create image folder
$(TEST_DST_DIR):
	mkdir -p $(TEST_DST_DIR)

# Create build folder
$(BUILD):
	mkdir -p $(BUILD)

# Strip unit-test binaries to reduce their size and copy them to image folder
$(TEST_DST_DIR)/%_test : $(TEST_SRC_DIR)/%_test | $(TEST_DST_DIR)
	$(TARGET)-strip -s -o $@ $<

# Copy resources/image
$(IMG_DIR)/etc: $(BUILD)
	cp -r $(BUILD_ROOT)/resources/image/* $(IMG_DIR)

# Create SD card image
BOOT_PART_SZ ?= 200
TEST_PART_SZ = $(firstword $(shell du -s -B 1MB $(IMG_DIR)))
IMG_SZ = $$(( $(BOOT_PART_SZ) + $(TEST_PART_SZ) + 10 ))
BOOT_FRACTION = $$(( ($(BOOT_PART_SZ) * 100) / $(IMG_SZ) ))
TEST_FRACTION = $$(( 100 - $(BOOT_FRACTION) ))

$(IMG): $(TESTS) $(IMG_DIR)/etc
	$(SDK_PREFIX)/common/prepare_hdd_img.sh \
        -s   $(IMG_SZ) -%1 $(BOOT_FRACTION) -%2 $(TEST_FRACTION) \
        -f1   $(IMG_FORMAT) \
        -f2   $(IMG_FORMAT) \
        -p1  /dev/zero \
        -p2  $(IMG_DIR) \
        -img $(IMG)

ifeq ($(strip $(TARGET_ARCH)),aarch64)
QEMU_OPTS = -m 2048 -machine vexpress-a15,secure=on -cpu cortex-a72 \
			-nographic -monitor none -smp 4 -serial stdio
QEMU_IMAGE_OPT = -drive file=$(IMG),if=sd,format=raw
QEMU_OPTS += $(QEMU_IMAGE_OPT)
else
$(error Bad target architecture: $(TARGET_ARCH). \
        No architectures other than aarch64 are currently supported)
endif

QEMU_SERIAL_PORT ?= 12345
QEMU_GDB_PORT ?= 1234
QEMU_GDB_OPTS = -serial tcp::$(QEMU_SERIAL_PORT),server,nowait \
				-S -gdb tcp::$(QEMU_GDB_PORT)

NPROC ?= 12

# For aarch64 target architecture, the SConstruct script sets the -march
# compiler option to armv8.2-a. KOS qemu does not support armv8.2-a. This
# causes some CPU instructions to fail.
# The most supported is armv8-a. Back to armv8-a.
MARCH=armv8-a

# TODO:
# 1) Switch the build to dynamic linking when supported by the KOS CE SDK;
.PHONY:  compile-mongo-tests
compile-mongo-tests:
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
		CCFLAGS="--sysroot=${SYSROOT_PATH} -march=${MARCH}" \
		TARGET_OS="${TARGET_OS}" \
		install-unittests -j${NPROC}


.PHONY: help dump-vars list-tests

help:
	@echo "The following targets are defined by this Makefile:"
	@echo "    help:                prints this message;"
	@echo "    dump-vars:           dump Makefile variables;"
	@echo "    compile-mongo-tests: compile and install mongo unittests;"
	@echo "    prebuild:            generate build files (Makefiles) from"
	@echo "                         CMakeLists.txt scenario;"
	@echo "    qemubuild:           build KOS image to run on QEMU;"
	@echo "    hwbuild:             build KOS image to run on RaspberryPi;"
	@echo "    qemurun:             run using QEMU"
	@echo "                         (options are specified in QEMU_OPTS "
	@echo "                         variable);"
	@echo "    qemurun/gdb:         run using QEMU. QEMU will be run with a GDB"
	@echo "                         server listening on tcp::QEMU_GDB_PORT"
	@echo "                         (GDB options are specified in QEMU_GDB_OPTS"
	@echo "                         variable);"
	@echo "    run/gdb:             run GDB, connect to server, add symbol file;"
	@echo "    clean:               aux. target (currently equal to clean-kos);"
	@echo "    clean-kos:           remove KOS build directory;"
	@echo "    clean-mongo:         remove MongoDB build directory;"
	@echo "    clean-all:           remove KOS & MongoDB build directories;"
	@echo ""
	@echo "The following targets are wrappers for those predefined by the KOS:"
	@echo "    sim:                 rebuild image and run using QEMU;"
	@echo "    sim/fast:            'fast' version of the sim target. Does the"
	@echo "                         same, except it DOESN'T rebuild an image;"
	@echo "    gdbsim:              rebuild image, run using QEMU. QEMU will be"
	@echo "                         run with a running GDB server waiting for a"
	@echo "                         connection on tcp::1234;"
	@echo "    gdbsim/fast:         'fast' version of the gdbsim target. Does the"
	@echo "                         same, except it DOESN'T rebuild an image;"
	@echo "    gdb:                 rebuild image and run GDB for target arch;"
	@echo "    gdb/fast:            'fast' version of the gdb target. Does the"
	@echo "                         same, except it DOESN'T rebuild an image;"


dump-vars:
	@echo "Variables:"
	@echo "    BUILD_ROOT = $(BUILD_ROOT)"
	@echo "    BUILD = $(BUILD)"
	@echo "    SDK_EDITION = $(SDK_EDITION)"
	@echo "    SDK_VERSION = $(SDK_VERSION)"
	@echo "    SDK_PREFIX = $(SDK_PREFIX)"
	@echo "    PATH = $(PATH)"
	@echo "    TARGET = $(TARGET)"
	@echo "    MONGO_SRC = $(MONGO_SRC)"
	@echo "    QEMU_IMAGE_OPT = $(QEMU_IMAGE_OPT)"
	@echo "    IMG_TEST_DIR = $(IMG_TEST_DIR)"
	@echo "    CHOSEN_TESTS = $(CHOSEN_TESTS)"

list-tests:
	@$(foreach name, $(sort $(notdir $(TESTS))), echo $(name);)

.PHONY: prebuild qemubuild hwbuild
prebuild: $(IMG)
	$(SDK_PREFIX)/toolchain/bin/cmake -G "Unix Makefiles" \
		-B $(BUILD) \
		-D CMAKE_BUILD_TYPE:STRING=Debug \
		-D CMAKE_INSTALL_PREFIX:STRING="$(INSTALL_PREFIX)" \
		-D CMAKE_TOOLCHAIN_FILE="$(SDK_PREFIX)/toolchain/share/toolchain-$(TARGET)$(TOOLCHAIN_SUFFIX).cmake" \
		-D QEMU_FLAGS="$(QEMU_IMAGE_OPT)" \
		-D IMG_TEST_DIR="$(IMG_TEST_DIR)" \
		-D CHOSEN_TESTS="$(CHOSEN_TESTS)" \
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

