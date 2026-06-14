set(src_files
    game.h game.c
    bomb_omb_battlefield.h bomb_omb_battlefield.c
    player.h player.c
    star.h star.c
    star_collected.h star_collected.c
    ui.h ui.c
    )

create_game(
    TARGET character_controller
    SOURCES ${src_files}
    EXTRA_LIBS fw64_components
    ALL_WARNINGS_AS_ERRORS)

    #target_compile_definitions(character_controller PRIVATE -DFW64_N64LIBULTRA_WAIT_FOR_START)
    target_compile_definitions(character_controller PRIVATE -DCOLLISION_TEST_NO_MUSIC)