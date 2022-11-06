set(src_files
    game.h game.c)

create_game(
    TARGET image_formats
    SOURCES ${src_files})