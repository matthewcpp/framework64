set(src_files
    game.h game.c)

create_game(
    TARGET data
    SOURCES ${src_files})