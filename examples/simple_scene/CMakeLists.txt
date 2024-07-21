set(src_files
    chase_cam.h chase_cam.c
    player.h player.c
    ui.h ui.c
    game.h game.c)

create_game(
    TARGET simple_scene
    SOURCES ${src_files}
    EXTRA_LIBS fw64_components
    ALL_WARNINGS_AS_ERRORS)