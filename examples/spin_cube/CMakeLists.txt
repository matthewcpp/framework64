set(src_files
    game.h game.c)

create_game(
    TARGET spin_cube
    SOURCES ${src_files})