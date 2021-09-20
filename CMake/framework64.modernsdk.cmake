set(CMAKE_C_COMPILER mips64-elf-gcc)
set(CMAKE_LINKER mips64-elf-ld)
set(CMAKE_C_LINK_EXECUTABLE "mips64-elf-ld --no-check-sections -o <TARGET>.o -r <OBJECTS>  /usr/lib/n64/nusys/nusys.o <LINK_LIBRARIES>")

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR mips64)

set(CMAKE_C_FLAGS "-mabi=32 -ffreestanding -mfix4300 -G 0")

include_directories(
	/usr/include/n64/nusys
	/etc/n64/usr/include/PR
	/usr/include/n64/nustd
	/usr/include/n64
)

link_directories(
	/etc/n64/usr/lib
	/usr/lib/n64/nusys
)

link_libraries (
	-lnualsgi_n 
	-ln_audio 
	-lnusys_d 
	-lultra_d
	/usr/local/n64chain/lib/gcc/mips64-elf/10.2.0/libgcc.a
	-lnustd
)

add_definitions(-DNU_DEBUG -DF3DEX_GBI_2 -DPLATFORM_N64)

set(spicy_path "/path/to/spicy")

function(create_n64_rom)
    set(options)
    set(oneValueArgs TARGET)
    set(multiValueArgs)
    cmake_parse_arguments(N64_ROM "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )
    set(target_name ${N64_ROM_TARGET})

    message(STATUS "Creating post-install steps to build N64 ROM: " ${target_name})

	set(specfile_src ${CMAKE_CURRENT_SOURCE_DIR}/${target_name}.spec)
	set(specfile_dest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${target_name}.spec)

	# copy the specfile into the build directory where the output file was generated
	add_custom_command(TARGET ${target_name} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy
		${specfile_src}
		${specfile_dest}
	)

	# build the rom with spicy
	add_custom_command(TARGET ${target_name} POST_BUILD
		WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
	    COMMAND spicy ${specfile_dest} -I/usr/include/n64/nusys -r ${target_name}.n64 -s 10 -e ${target_name}.out
	)

	# mask the rom
	add_custom_command(TARGET ${target_name} POST_BUILD
		WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
	    COMMAND makemask ${target_name}.n64
	)
endfunction()

set(FW64_TARGET_PLATFORM_N64)
