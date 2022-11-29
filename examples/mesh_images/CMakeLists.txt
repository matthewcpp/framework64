set(src_files
    game.h game.c)

create_game(
    TARGET mesh_images
    SOURCES ${src_files}
    EXTRA_LIBS example_common)