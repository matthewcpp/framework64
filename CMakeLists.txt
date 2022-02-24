cmake_minimum_required(VERSION 3.16)

option(Fw64_BUILD_TESTS "Builds the gtest based test applications" OFF)
option(FW64_BUILD_EXAMPLES "builds the example applications" OFF)

if (NOT DEFINED FW64_PLATFORM_CONFIGURED)
    include(cmake/configure_platform.cmake REQUIRED)
endif()

if (FW64_PLATFORM_N64)
    project(framework64 C)
else()
    project(framework64)
endif()

add_subdirectory(src)

if (FW64_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if (FW64_PLATFORM_DESKTOP AND Fw64_BUILD_TESTS)
    add_subdirectory(tests)
endif()
