project (mongod)

# Tools for using NK parser.
include (platform/nk)

# Set compile flags.
project_header_default ("STANDARD_GNU_11:YES" "STRICT_WARNINGS:NO")

nk_build_edl_files (mongod_edl_files
                    NK_MODULE "mongod"
                    EDL "${CMAKE_SOURCE_DIR}/resources/edl/Mongod.edl")

add_executable (Mongod IMPORTED GLOBAL)
# TODO: fix path
set_property(TARGET Mongod PROPERTY IMPORTED_LOCATION
    ${CMAKE_SOURCE_DIR}/../build/install/bin/mongod)

add_dependencies (Mongod mongod_edl_files)
target_link_libraries (Mongod)

