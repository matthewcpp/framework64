set(src_files
    game.h game.c)

create_game(
    TARGET level_tiles
    SOURCES ${src_files}
    EXTRA_LIBS example_common
)