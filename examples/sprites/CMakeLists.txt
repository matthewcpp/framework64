set(src_files
    game.h game.c
    sprites.h sprites.c)

create_game(
    TARGET sprites
    SOURCES ${src_files}
    ALL_WARNINGS_AS_ERRORS)