cmake_minimum_required(VERSION 3.16)

option(Fw64_BUILD_TESTS "Builds the gtest based test applications" ON)
option(FW64_BUILD_EXAMPLES "Builds the example applications" ON)

option(FW64_BUILD_TOOLS "Builds the companion tools" ON)

option(FW64_BUILD_DATALINK_CLIENT_N64 "Builds the datalink client for n64" ON)

option(FW64_MEDIA_ENABLED "Enables fw64 media module" ON)

if (NOT DEFINED FW64_PLATFORM_CONFIGURED)
    include(cmake/configure_platform.cmake REQUIRED)
endif()

if (FW64_PLATFORM_N64)
    project(framework64 C)
else()
    project(framework64)
endif()

add_subdirectory(src)

if (FW64_BUILD_EXAMPLES OR FW64_BUILD_TOOLS)
    add_subdirectory(components)
    set(FW64_COMPONENTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/components)
endif()

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
