set(src_files
    game.h game.c)

create_game(
    TARGET character_animation
    SOURCES ${src_files}
    EXTRA_LIBS example_common)