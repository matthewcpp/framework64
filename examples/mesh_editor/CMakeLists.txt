set(src_files
    game.h game.c)

create_game(
    TARGET mesh_editor
    SOURCES ${src_files})