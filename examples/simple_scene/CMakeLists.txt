set(src_files
    ../billboard/flame.h ../billboard/flame.c
    chase_cam.h chase_cam.c
    player.h player.c
    ui.h ui.c
    game.h game.c )

    create_game(
        TARGET simple_scene
        SOURCES ${src_files}
        EXTRA_LIBS fw64_components)