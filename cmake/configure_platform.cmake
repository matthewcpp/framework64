# determine's the root directory path of FW64 Repo.  THis can vary whether the script is running from
# the framework64 directory directly or included as a submodule from within a game.
if(EXISTS "${CMAKE_SOURCE_DIR}/pipeline/Pipeline.js")  # running from within the framework64 source directory
    set(FW64_ROOT_DIR ${CMAKE_SOURCE_DIR} CACHE INTERNAL "FW64_ROOT_DIR")
else()
    set(FW64_ROOT_DIR ${CMAKE_SOURCE_DIR}/lib/framework64 CACHE INTERNAL "FW64_ROOT_DIR")
endif()

# Add cmake files that are needed by all platforms
include(${FW64_ROOT_DIR}/cmake/add_game_modules.cmake)

# configure the target platform
if (FW64_PLATFORM_N64_LIBULTRA)
    message("Configuring for Platform: N64_libultra")
    set(CMAKE_TOOLCHAIN_FILE ${FW64_ROOT_DIR}/cmake/framework64.libultra.cmake)
elseif(FW64_PLATFORM_DESKTOP)
    message("Configuring for Platform: Desktop")
    set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE ${FW64_ROOT_DIR}/cmake/framework64.desktop.cmake)
else()
    message(FATAL_ERROR "Unable to determine FW64 Platform")
endif()

set(FW64_PLATFORM_CONFIGURED TRUE)