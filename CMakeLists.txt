cmake_minimum_required(VERSION 3.16)

option(Fw64_BUILD_TESTS "Builds the gtest based test applications" OFF)
option(FW64_BUILD_EXAMPLES "builds the example applications" OFF)

if (FW64_PLATFORM_N64)
    message("Configuring for Platform: Nintendo64")
    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/CMake/framework64.modernsdk.cmake)
    project(framework64 C)
else()
    message("Configuring for Platform: Desktop")
    project(framework64)
    include(${CMAKE_CURRENT_SOURCE_DIR}/CMake/framework64.desktop.cmake)
endif()

add_subdirectory(src)

if (FW64_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if (FW64_PLATFORM_DESKTOP AND BUILD_FW64_TESTS)
    add_subdirectory(tests)
endif()
