set(src_files
    game.h game.c)

create_game(
    TARGET bounding
    SOURCES ${src_files}
    ALL_WARNINGS_AS_ERRORS)