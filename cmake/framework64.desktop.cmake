set(FW64_PLATFORM_DESKTOP ON)
set(CMAKE_CXX_STANDARD 17)

# performs platform specific configuration of the framework 64 library
function (configure_core_library)
    find_package(GLEW REQUIRED)
    find_package(nlohmann_json CONFIG REQUIRED)
    find_package(SDL2 CONFIG REQUIRED)
    find_package(sdl2_mixer CONFIG REQUIRED)
    find_package(sdl2-image CONFIG REQUIRED)
    find_package(unofficial-sqlite3 CONFIG REQUIRED)

    target_link_libraries(framework64 PUBLIC GLEW::GLEW)
    # mac SDL mixer?: SDL2_mixer::SDL2_mixer-static
    target_link_libraries(framework64 PUBLIC SDL2::SDL2 SDL2::SDL2main SDL2::SDL2_image)
    target_link_libraries(framework64 PUBLIC $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>)
    target_link_libraries(framework64 PUBLIC unofficial::sqlite3::sqlite3)

    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)

    # this is used on windows for the DLL utility copying script
    if (WIN32)
        file(WRITE ${CMAKE_BINARY_DIR}/build_info.json "{\"target\":\"${VCPKG_TARGET_TRIPLET}\",\"type\":\"${CMAKE_BUILD_TYPE}\"}")
    endif()
endfunction()

# performs platform specific configuration of a framework64 game
function(create_game)
    set(options)
    set(oneValueArgs TARGET SAVE_FILE_TYPE)
    set(multiValueArgs SOURCES EXTRA_LIBS)
    cmake_parse_arguments(DESKTOP_GAME "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

    set(target_name ${DESKTOP_GAME_TARGET})
    set(game_sources ${DESKTOP_GAME_SOURCES})

    add_executable(${target_name} ${game_sources} ${FW64_ROOT_DIR}/src/desktop/main_desktop.cpp)
    target_link_libraries(${target_name} PUBLIC framework64)

    # Configure target specific output directories
    set(output_dir ${CMAKE_BINARY_DIR}/bin/${target_name})
    set_target_properties(${target_name} PROPERTIES 
        RUNTIME_OUTPUT_DIRECTORY ${output_dir}
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${output_dir}
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${output_dir})

    # include the game specific asset directory
    set(asset_include_dir ${output_dir}/assets/include)
    target_include_directories(${target_name} PUBLIC ${asset_include_dir})

    # Add any extra libraries that need to be linked in
    if (DEFINED DESKTOP_GAME_EXTRA_LIBS)
        target_link_libraries(${target_name} PUBLIC ${DESKTOP_GAME_EXTRA_LIBS})
    endif()

    # Fixes linker error encountered on linux
    # https://stackoverflow.com/a/55086637
    if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        target_link_options(${target_name} PUBLIC "-Wl,--copy-dt-needed-entries")
    endif()

    target_include_directories(${target_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

    if (NOT DEFINED DESKTOP_GAME_SAVE_FILE_TYPE)
        set(DESKTOP_GAME_SAVE_FILE_TYPE "NONE")
    endif()

    target_compile_definitions(${target_name} 
        PUBLIC FW64_APPLICATION_NAME="${target_name}"
        PUBLIC FW64_SAVE_FILE_TYPE="${DESKTOP_GAME_SAVE_FILE_TYPE}")

endfunction()