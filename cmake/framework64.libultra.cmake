# Note: Helpful resource: https://gcc.gnu.org/onlinedocs/gcc/Option-Summary.html

set(CMAKE_C_COMPILER mips-n64-gcc)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR mips64)

set(CMAKE_C_FLAGS "-mabi=32 -ffreestanding -mfix4300 -G 0")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DNU_DEBUG)
else()
    add_definitions(-D_FINALROM)
endif()

add_definitions(-DF3DEX_GBI_2 -DFW64_PLATFORM_N64_LIBULTRA)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) # note: roms will be copied into this directory as post build step

set(FW64_N64_ASM_SRC_DIR ${FW64_ROOT_DIR}/src/framework64/n64_libultra/asm)

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

# performs platform specific configuration of the framework 64 library
function (configure_core_library)
    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(framework64 PUBLIC -O0)
    endif()

    # target_compile_options(${target_name} PUBLIC -G 0 -mabi=32 -ffreestanding -mfix4300)
    target_include_directories(framework64 
        PUBLIC /usr/include/n64 /usr/include/n64/PR  /usr/include/n64/nusys /usr/include/n64/nustd)

        enable_all_warnings_as_errors(TARGET framework64)

endfunction()

# performs platform specific configuration of a framework64 game
function(create_game)
    set(options ALL_WARNINGS_AS_ERRORS)
    set(oneValueArgs TARGET SAVE_FILE_TYPE GAME_HEADER_PATH)
    set(multiValueArgs SOURCES EXTRA_LIBS)
    cmake_parse_arguments(N64_ROM "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

    # set the correct EEPROM type for the rom
    # default setting is no EEPROM
    set(FW64_N64_ROM_ID ".ascii \"  \"")
    set(FW64_N64_SETTINGS_BYTE ".ascii \" \"")

    if (DEFINED N64_ROM_SAVE_FILE_TYPE)
        set(FW64_N64_ROM_ID ".ascii \"ED\"")

        if ("${N64_ROM_SAVE_FILE_TYPE}" STREQUAL "N64_EEPROM_4K")
            set(FW64_N64_SETTINGS_BYTE ".byte 0x12")
        elseif("${N64_ROM_SAVE_FILE_TYPE}" STREQUAL "N64_EEPROM_16K")
            set(FW64_N64_SETTINGS_BYTE ".byte 0x22")
        else()
            message(FATAL_ERROR "Unsupported Save File type specified for ${N64_ROM_TARGET}: ${N64_ROM_SAVE_FILE_TYPE}")
        endif()
    endif()

    set(target_name ${N64_ROM_TARGET})
    set(game_sources ${N64_ROM_SOURCES})
    set(game_asset_dir ${CMAKE_SOURCE_DIR}/build_n64_libultra/bin/${target_name}/assets)

	# copy the assembly files into the build dir and add them to the target
	set(asm_dest_dir ${CMAKE_CURRENT_BINARY_DIR}/asm)
	file(MAKE_DIRECTORY ${asm_dest_dir})
	configure_file(${FW64_N64_ASM_SRC_DIR}/macros.inc ${asm_dest_dir}/macros.inc COPYONLY)
	configure_file(${FW64_N64_ASM_SRC_DIR}/entry.s ${asm_dest_dir}/entry.s COPYONLY)
	configure_file(${FW64_N64_ASM_SRC_DIR}/rom_header.s ${asm_dest_dir}/rom_header.s)
	configure_file(${FW64_N64_ASM_SRC_DIR}/asset_data.s ${asm_dest_dir}/asset_data.s)

	set(asm_files
		${asm_dest_dir}/entry.s
		${asm_dest_dir}/rom_header.s
		${asm_dest_dir}/asset_data.s
	)

    if (DEFINED N64_ROM_GAME_HEADER_PATH)
        set(game_include_path ${N64_ROM_GAME_HEADER_PATH})
    else()
        set(game_include_path "game.h")
    endif()

    set(main_file_src ${FW64_ROOT_DIR}/src/framework64/n64_libultra/main_n64.c)
    set(main_file_dest ${CMAKE_CURRENT_BINARY_DIR}/main_n64_${target_name}.c)
    configure_file(${main_file_src} ${main_file_dest})
    
    add_executable(${target_name} ${game_sources} ${main_file_dest} ${asm_files})

    # Include the game specific asset directory
    target_include_directories(${target_name} PUBLIC ${game_asset_dir}/include)

    # Add any extra libraries that need to be linked in
    if (DEFINED N64_ROM_EXTRA_LIBS)
        target_link_libraries(${target_name} PUBLIC ${N64_ROM_EXTRA_LIBS})
    endif()

    target_link_libraries(${target_name} PUBLIC framework64)

    set(elf_output_name ${target_name}.elf)
    set_target_properties(${target_name} PROPERTIES OUTPUT_NAME ${elf_output_name})

    # configure assembly compile options
    set(asm_compile_options "-x;assembler-with-cpp;-MMD;-Wa,-I${asm_dest_dir}")
    set_source_files_properties(${asm_dest_dir}/entry.s PROPERTIES LANGUAGE C COMPILE_OPTIONS "${asm_compile_options}")
    set_source_files_properties(${asm_dest_dir}/rom_header.s PROPERTIES LANGUAGE C COMPILE_OPTIONS "${asm_compile_options}")
    set_source_files_properties(${asm_dest_dir}/asset_data.s PROPERTIES LANGUAGE C COMPILE_OPTIONS "${asm_compile_options}")

    # prepare bootcode object
    set(bootcode_src /usr/lib/n64/PR/bootcode/boot.6102)
    set(bootcode_dest ${CMAKE_CURRENT_BINARY_DIR}/boot.6102.o)
    execute_process(COMMAND mips-n64-objcopy -I binary -B mips -O elf32-bigmips ${bootcode_src} ${bootcode_dest})

    # set assembly file paths to configure linkerscript
    # Note: cmake is passing the paths to these files relative to the current binary directory.  The paths we configure should match the path on the linker command line
    set(obj_folder_root CMakeFiles/${target_name}.dir)
    set(entry_path ${obj_folder_root}/asm/entry.s.obj)
    set(rom_header_path ${obj_folder_root}/asm/rom_header.s.obj)
    set(asset_data_path ${obj_folder_root}/asm/asset_data.s.obj)
    set(bootcode_path boot.6102.o)

    # create the linkerscript file containing the paths to the built obects
    set(main_obj_dir ${obj_folder_root})
    set(configured_linkerfile_path ${CMAKE_CURRENT_BINARY_DIR}/${target_name}.ld.in)
    configure_file(${FW64_ROOT_DIR}/src/framework64/n64_libultra/linkerscript.ld.in ${configured_linkerfile_path})

    # Run the linker file though the preprocessor
    set(linkerscript_dest ${CMAKE_CURRENT_BINARY_DIR}/${target_name}.ld)
    execute_process(COMMAND cpp -P -Wno-trigraphs -I/usr/include/n64 -I/usr/include/n64/PR -I/usr/include/n64/nusys -D_FINALROM=1 -DNDEBUG=1 -DF3DEX_GBI_2=1 -MMD -MP -o ${linkerscript_dest} ${configured_linkerfile_path})

    # configure target specific build options 
    target_include_directories(${target_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_directories(${target_name} PUBLIC /usr/lib/n64/nusys /usr/lib/n64)
    target_link_libraries(${target_name} PUBLIC -lnualsgi_n -ln_audio -lnusys -lultra_rom -lnustd $ENV{N64_LIBGCCDIR}/libgcc.a)
    target_link_options(${target_name} PUBLIC -T ${linkerscript_dest} -Wl,--no-check-sections ${bootcode_path} /usr/lib/n64/nusys/nusys_rom.o)

    # build the rom
    set(rom_elf_src ${CMAKE_CURRENT_BINARY_DIR}/${elf_output_name})
    set(final_rom_path ${CMAKE_BINARY_DIR}/bin/${target_name}.z64)

    add_custom_command(TARGET ${target_name} POST_BUILD
        COMMAND mips-n64-objcopy --pad-to=0x100000 --gap-fill=0xFF ${rom_elf_src} ${final_rom_path} -O binary
    )

    # mask the rom
    add_custom_command(TARGET ${target_name} POST_BUILD
        COMMAND makemask ${final_rom_path}
    )

endfunction()