set(src_files
    game.h game.c)

create_game(
    TARGET mesh_images
    SOURCES ${src_files}
    EXTRA_LIBS fw64_components)