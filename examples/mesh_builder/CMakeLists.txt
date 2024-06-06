set(src_files
    game.h game.c)

create_game(
    TARGET mesh_builder
    SOURCES ${src_files}
    ALL_WARNINGS_AS_ERRORS)