set(src_files
    ../common/${platform_main_file}
    game.h game.c)

add_executable(terrain ${src_files})

target_link_libraries(terrain framework64)
target_include_directories(terrain
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../common
        PRIVATE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets)

if (FW64_PLATFORM_N64)
    create_n64_rom(TARGET terrain)
endif()