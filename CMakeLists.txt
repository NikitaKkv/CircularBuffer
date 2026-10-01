cmake_minimum_required(VERSION 3.12)

project(
    circular_buffer
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(lib)

add_subdirectory(bin)

enable_testing()
add_subdirectory(tests)

# Do not add or modify anything below this line
include(tests/coverage.cmake)
