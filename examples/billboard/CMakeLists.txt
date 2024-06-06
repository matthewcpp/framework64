set(src_files
    flame.h flame.c
    game.h game.c)

create_game(
    TARGET billboard
    SOURCES ${src_files}
    EXTRA_LIBS fw64_components
    ALL_WARNINGS_AS_ERRORS)