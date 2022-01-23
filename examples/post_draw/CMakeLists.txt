set(src_files
    game.h game.c)

create_game(
    TARGET post_draw
    SOURCES ${src_files})