if(EXISTS "${CMAKE_SOURCE_DIR}/pipeline/Prepare.js")  # running from within the framework64 source directory
    set(FW64_ROOT_DIR ${CMAKE_SOURCE_DIR} CACHE INTERNAL "FW64_ROOT_DIR")
else()
    set(FW64_ROOT_DIR ${CMAKE_SOURCE_DIR}/lib/framework64 CACHE INTERNAL "FW64_ROOT_DIR")
endif()

if (FW64_PLATFORM_N64)
    message("Configuring for Platform: N64-libultra")
    set(CMAKE_TOOLCHAIN_FILE ${FW64_ROOT_DIR}/cmake/framework64.modernsdk.cmake)
else()
    message("Configuring for Platform: Desktop")
    set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE ${FW64_ROOT_DIR}/cmake/framework64.desktop.cmake)
endif()

set(FW64_PLATFORM_CONFIGURED TRUE)