set(src_files
    game.h game.c
    player.h player.c
    ui.h ui.c
    fw64_character.h fw64_character.c
    )

create_game(
    TARGET collision_test
    SOURCES ${src_files}
    EXTRA_LIBS fw64_components
    ALL_WARNINGS_AS_ERRORS)