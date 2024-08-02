set(src_files
    ambient_editor.h ambient_editor.c
    color_editor.h color_editor.c
    light_editor.h light_editor.c
    material_editor.h material_editor.c
    mesh_picker.h mesh_picker.c
    ui.h ui.c
    game.h game.c)

create_game(
    TARGET lighting
    SOURCES ${src_files}
    EXTRA_LIBS fw64_components
    ALL_WARNINGS_AS_ERRORS)