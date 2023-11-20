set(src_files
    game.h game.c)

create_game(
    TARGET fog
    SOURCES ${src_files}
    EXTRA_LIBS fw64_components)