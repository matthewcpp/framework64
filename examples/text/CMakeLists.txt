set(src_files
    game.h game.c)

create_game(
    TARGET text
    SOURCES ${src_files}
    ALL_WARNINGS_AS_ERRORS)