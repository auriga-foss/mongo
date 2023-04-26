cmake_minimum_required (VERSION 3.12)
project (mongo-tests)

# Initialize CMake library for Kaspersky OS SDK.
include (platform)
initialize_platform (FORCE_STATIC)

# Add functions for installing artefacts.
include (platform/install)

# Toolchain for work with NK-parser.
include (platform/nk)

find_package(ns REQUIRED)

# Add a package for working with the virtual file system.
find_package (vfs REQUIRED)
include_directories (${vfs_INCLUDE})

# Attach a package with VFS entities implementation
find_package (precompiled_vfs REQUIRED)
include_directories (${precompiled_vfs_INCLUDE})

# Attach a package with the dhcpcd entity implementation.
# TODO: Need to check if Mongo unit-tests use networking and remove it, if not
find_package (rump REQUIRED COMPONENTS DHCPCD_ENTITY)
include_directories (${rump_INCLUDE})

# Set directory where unit-test executables will be located
set (MONGO_TEST_DIR ${IMG_TEST_DIR})

# Set unit-test executables to be run for none-empty var
if(NOT "${CHOSEN_TESTS}" STREQUAL "")
    string(REPLACE " " ";" MONGO_TEST_BINARIES ${CHOSEN_TESTS})
endif()

add_subdirectory (test_runner)
add_subdirectory (einit)

