set(src_files
    game.h game.c)

create_game(
    TARGET viewport
    SOURCES ${src_files})