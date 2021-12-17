set(FW64_PLATFORM_DESKTOP ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/pipeline/Prepare.js")  # running from within the framework64 source directory
    set(FW64_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
else()
    set(FW64_ROOT_DIR ${CMAKE_SOURCE_DIR}/lib/framework64)
endif()

# this is temporary and not ideal todo: figure out a bettwer way to deal with this
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) 
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)

set(FW64_DESKTOP_ASSET_DIR ${CMAKE_BINARY_DIR}/bin/assets)

find_package(GLEW REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(sdl2-mixer CONFIG REQUIRED)
find_package(sdl2-image CONFIG REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)

file(WRITE ${CMAKE_BINARY_DIR}/build_info.json "{\"sizeOfVoidP\":${CMAKE_SIZEOF_VOID_P},\"type\":\"${CMAKE_BUILD_TYPE}\"}")

# performs platform specific configuration of the framework 64 library
function (configure_core_library)
    target_link_libraries(framework64 PUBLIC GLEW::GLEW)
    target_link_libraries(framework64 PUBLIC SDL2::SDL2 SDL2::SDL2main SDL2::SDL2_image SDL2::SDL2_mixer)
    target_link_libraries(framework64 PUBLIC unofficial::sqlite3::sqlite3)

    target_include_directories(framework64 PUBLIC ${FW64_DESKTOP_ASSET_DIR}/include)
endfunction()

# performs platform specific configuration of a framework64 game
function(create_game)
    set(options)
    set(oneValueArgs TARGET)
    set(multiValueArgs SOURCES)
    cmake_parse_arguments(DESKTOP_GAME "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

    set(target_name ${DESKTOP_GAME_TARGET})
    set(game_sources ${DESKTOP_GAME_SOURCES})

    add_executable(${target_name} ${game_sources} ${FW64_ROOT_DIR}/src/desktop/main_desktop.cpp)
    target_link_libraries(${target_name} PUBLIC framework64)

    target_include_directories(${target_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

endfunction()