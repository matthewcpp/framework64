set(src_files
    ../common/${platform_main_file}
    game.h game.c)

add_executable(spin_cube ${src_files})

target_link_libraries(spin_cube framework64)
target_include_directories(spin_cube
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../common
    PRIVATE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/include)

if (FW64_PLATFORM_N64)
    create_n64_rom(TARGET spin_cube)
endif()