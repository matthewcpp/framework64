set(src_files
    game.h game.c)

create_game(
    TARGET audio_banks
    SOURCES ${src_files}
    ALL_WARNINGS_AS_ERRORS)