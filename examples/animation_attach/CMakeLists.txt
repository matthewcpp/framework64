set(src_files
    game.h game.c)

create_game(
    TARGET animation_attach
    SOURCES ${src_files}
    EXTRA_LIBS example_common)