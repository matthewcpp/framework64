set(src_files
    ../common/${platform_main_file}
    ../common/flame.h ../common/flame.c
        chase_cam.h chase_cam.c
    player.h player.c
    ui.h ui.c
    game.h game.c )

add_executable(simple_scene ${src_files})

target_link_libraries(simple_scene framework64)
target_include_directories(simple_scene
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../common
        PRIVATE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets)

if (FW64_PLATFORM_N64)
    create_n64_rom(TARGET simple_scene)
endif()