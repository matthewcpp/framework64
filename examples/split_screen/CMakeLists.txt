set(src_files
    game.h game.c
    player.h player.c)

create_game(
    TARGET split_screen
    SOURCES ${src_files}
    EXTRA_LIBS example_common)