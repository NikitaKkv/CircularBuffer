include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/releases/download/v1.16.0/googletest-1.16.0.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP ON
    FIND_PACKAGE_ARGS NAMES GTest
)

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Option to enable extendable buffer tests
# To run extendable buffer tests, set RUN_EXT_TESTS to ON and rebuild from scratch
option(RUN_EXT_TESTS "Enable extendable buffer tests" ON)

add_executable(
    circular-buffer-lib-tests
    named_requirements_ut.cpp
    simple_ut.cpp
    no_default_constructible_ut.cpp
    allocator_ut.cpp
    exception_tests.cpp
    other_tests.cpp
)

if(RUN_EXT_TESTS)
    target_sources(circular-buffer-lib-tests PRIVATE ext_buffer_ut.cpp)
    target_compile_definitions(circular-buffer-lib-tests PRIVATE RUN_EXT_TESTS)
endif()

target_link_libraries(
    circular-buffer-lib-tests
    GTest::gtest_main
    GTest::gmock_main
)

target_include_directories(circular-buffer-lib-tests PUBLIC ${PROJECT_SOURCE_DIR})

include(GoogleTest)

gtest_discover_tests(circular-buffer-lib-tests)
