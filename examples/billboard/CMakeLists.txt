set(src_files
    flame.h flame.c
    game.h game.c)

create_game(
    TARGET billboard
    SOURCES ${src_files})