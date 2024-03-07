set(src_files
    game.h game.c)

create_game(
    TARGET project_unproject
    SOURCES ${src_files}
    EXTRA_LIBS fw64_components)