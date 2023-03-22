cmake_minimum_required (VERSION 3.12)
project (node)

# Initialize CMake library for Kaspersky OS SDK.
include (platform)
initialize_platform (FORCE_STATIC)

# Add functions for installing artefacts.
include (platform/install)

# Toolchain for work with NK-parser.
include (platform/nk)

# Add a package for working with the virtual file system.
find_package (vfs REQUIRED)
include_directories (${vfs_INCLUDE})

# Attach a package with VFS entities implementation
find_package (precompiled_vfs REQUIRED)
include_directories (${precompiled_vfs_INCLUDE})

add_subdirectory (mongod)
add_subdirectory (einit)

