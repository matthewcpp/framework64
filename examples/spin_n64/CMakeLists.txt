set(src_files
    game.h game.c)

create_game(
    TARGET spin_n64
    SOURCES ${src_files})