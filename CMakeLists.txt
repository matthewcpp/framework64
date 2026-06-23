cmake_minimum_required(VERSION 3.16)

option(FW64_BUILD_EXAMPLES "Builds the example applications" ON)

option(Fw64_BUILD_TESTS "Builds the gtest based test applications" ON)
option(FW64_BUILD_TOOLS "Builds the companion tools" ON)
option(FW64_BUILD_DATALINK_CLIENT_N64 "Builds the datalink client for n64" ON)

if (EMSCRIPTEN)
    set(Fw64_BUILD_TESTS OFF)
    set(FW64_BUILD_TOOLS OFF)
    set(FW64_BUILD_DATALINK_CLIENT_N64 OFF)
endif()

if (NOT DEFINED FW64_PLATFORM_CONFIGURED)
    include(cmake/configure_platform.cmake REQUIRED)
endif()

if (FW64_PLATFORM_N64_LIBULTRA)
    project(framework64 C)
else()
    project(framework64)
endif()

add_subdirectory(src/framework64)
add_subdirectory(src/modules)
add_subdirectory(components)

if (FW64_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if (FW64_BUILD_TOOLS)
    add_subdirectory(tools)
endif()

if (FW64_PLATFORM_DESKTOP)
    if (Fw64_BUILD_TESTS)
        add_subdirectory(tests)
    endif()
endif()
