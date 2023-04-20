project (test_runner)

include (platform/nk)

project_header_default ("STANDARD_GNU_11:YES" "STRICT_WARNINGS:NO")

nk_build_edl_files (
    test_runner_edl_files
    NK_MODULE test_runner
    EDL "${CMAKE_SOURCE_DIR}/resources/edl/Testrunner.edl")

nk_build_edl_files (
        test_edl_files
        NK_MODULE test_runner
        EDL "${CMAKE_SOURCE_DIR}/resources/edl/Test.edl")

add_executable (Testrunner "src/test_runner.c" )
target_link_libraries (Testrunner ${${PROJECT_NAME}_LIBS} ${vfs_CLIENT_LIB})

# Set test directory as argument 
set_target_properties (Testrunner PROPERTIES EXTRA_ARGS "  - ${MONGO_TEST_DIR}")

add_dependencies (Testrunner test_runner_edl_files test_edl_files)
