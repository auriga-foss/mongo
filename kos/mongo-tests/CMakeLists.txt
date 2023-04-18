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
find_package (rump REQUIRED COMPONENTS DHCPCD_ENTITY)
include_directories (${rump_INCLUDE})

# Set directory where unit-test executables are located
set (MONGO_TEST_DIR "/tests")

# Set unit-test executables to be run
set (MONGO_TEST_BINARIES 
        "db_bson_test"
        "db_sorter_test")

add_subdirectory (test_runner)
add_subdirectory (einit)

