set(src_files
    game.h game.c)

create_game(
    TARGET mesh_builder
    SOURCES ${src_files})