cmake_minimum_required(VERSION 3.16)

option(BUILD_Fw64_TESTS "Builds the gtest based test applications" OFF)
option(BUILD_FW64_EXAMPLES "builds the example applications" OFF)

if (FW64_PLATFORM_N64)
    message("Configuring for Platform: Nintendo64")
    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/CMake/framework64.modernsdk.cmake)
    project(framework64 C)
else()
    message("Configuring for Platform: Desktop")
    project(framework64)
    set(CMAKE_CXX_STANDARD 17)
    set(FW64_PLATFORM_DESKTOP ON)

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

    set(platform_main_file main_desktop.cpp)
    set(platform_assets_dir ${PROJECT_SOURCE_DIR}/build_desktop/bin/assets)
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(src)

if (FW64_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if (FW64_PLATFORM_DESKTOP AND BUILD_FW64_TESTS)
    add_subdirectory(tests)
endif()
