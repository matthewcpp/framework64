set(src_files
    ../common/${platform_main_file}
    ../common/flame.h ../common/flame.c
    game.h game.c)

add_executable(billboard ${src_files})

target_link_libraries(billboard framework64)
target_include_directories(billboard
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../common
    PRIVATE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/include)

if (FW64_PLATFORM_N64)
    create_n64_rom(TARGET billboard)
endif()