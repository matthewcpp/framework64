set(src_files
    color_editor.h color_editor.c
    material_editor.h material_editor.c
    game.h game.c)

create_game(
    TARGET lighting
    SOURCES ${src_files}
    EXTRA_LIBS fw64_components
    ALL_WARNINGS_AS_ERRORS)