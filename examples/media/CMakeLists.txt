set(src_files
    game.h game.c)

create_game(
    TARGET media
    ENABLE_MEDIA
    SOURCES ${src_files}
    EXTRA_LIBS fw64_components
    ALL_WARNINGS_AS_ERRORS)

add_game_modules(TARGET media MODULES media)

# Note this is used here to avoid a clash with the media directory having the same name as the example executable
if (FW64_PLATFORM_DESKTOP)
    target_compile_definitions(media PRIVATE FW64_MEDIA_DIR_NAME="media_root")
endif()

