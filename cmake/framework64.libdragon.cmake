
set(CMAKE_C_COMPILER /opt/libdragon/bin/mips64-elf-gcc)

set(FW64_TARGET_PLATFORM_N64_LIBDRAGON)
set(fw64_libdragon_src_dir ${FW64_ROOT_DIR}/src/framework64/n64_libdragon)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib) # note: roms will be copied into this directory as post build step

function (configure_core_library)
    target_include_directories(framework64 PUBLIC /opt/libdragon/mips64-elf/include)
    target_compile_options(framework64 PUBLIC -march=vr4300 -mtune=vr4300 -falign-functions=32 -ffunction-sections -fdata-sections -g -O2)
    target_compile_definitions(framework64 PUBLIC FW64_PLATFORM_N64_LIBDRAGON -DN64)
endfunction()

function (enable_all_warnings_as_errors)
    set(options)
    set(oneValueArgs TARGET)
    set(multiValueArgs)
    cmake_parse_arguments(ENABLE_ALL_WARNINGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

    set(target_name ${ENABLE_ALL_WARNINGS_TARGET})

    # Turn this on when CI is enabled
    set_property(TARGET ${target_name} PROPERTY COMPILE_WARNING_AS_ERROR ON)
    target_compile_options(${target_name} PRIVATE -Wall -Wextra -Wpedantic)

endfunction()

# performs platform specific configuration of a framework64 game
function(create_game)
    set(options ALL_WARNINGS_AS_ERRORS)
    set(oneValueArgs TARGET SAVE_FILE_TYPE)
    set(multiValueArgs SOURCES EXTRA_LIBS)
    cmake_parse_arguments(N64_ROM "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

    set(libdragon_game_makefile_in ${fw64_libdragon_src_dir}/Makefile.in)

    set(target_name ${N64_ROM_TARGET})
    set(game_sources ${N64_ROM_SOURCES})
    set(game_binary_dir ${CMAKE_SOURCE_DIR}/build_n64_libdragon/bin/${target_name})
    set(game_asset_dir ${game_binary_dir}/assets)

    # Create static library containing all game logic.
    # This will be linked in to the platform main file
    # ROM will be built using the standard libdragon makefile
    add_library(${target_name} STATIC ${game_sources})
    target_link_libraries(${target_name} PUBLIC framework64)

     # Include the game specific asset directory
     target_include_directories(${target_name} PUBLIC ${game_asset_dir}/include)

    # Add any extra libraries that need to be linked in
    if (DEFINED N64_ROM_EXTRA_LIBS)
        target_link_libraries(${target_name} PUBLIC ${N64_ROM_EXTRA_LIBS})
    endif()

    # Setup the game's libdragon makefile
    set(fw64_libdragon_lib_search_dir ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
    set(fw64_include_dir ${FW64_ROOT_DIR}/src)
    set(fw64_current_game_dir ${CMAKE_CURRENT_SOURCE_DIR})
    set(target_dfs_bundle ${game_binary_dir}/assets/assets.dfs)
    configure_file(${fw64_libdragon_src_dir}/Makefile.in ${game_binary_dir}/Makefile)

    # Configure the game's main file
    set(main_file_src ${fw64_libdragon_src_dir}/src/framework64/n64_libdragon/main_n64.c)
    set(main_file_dest ${CMAKE_CURRENT_BINARY_DIR}/main_n64_${target_name}.c)

    configure_file(${fw64_libdragon_src_dir}/main_n64_libdragon.c ${game_binary_dir}/main_n64_libdragon.c COPYONLY)

    add_custom_command(TARGET ${target_name} 
        POST_BUILD COMMAND make
        WORKING_DIRECTORY ${game_binary_dir}
    )

endfunction()