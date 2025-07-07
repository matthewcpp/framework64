set(src_files
    game.h game.c)

create_game(
    TARGET media
    SOURCES ${src_files}
    STATIC_MODULES media
    EXTRA_LIBS fw64_components
    ALL_WARNINGS_AS_ERRORS)

# Note this is used here to avoid a clash with the media directory having the same name as the example executable
set(media_dir_name "media_root")
if (FW64_PLATFORM_DESKTOP)
    target_compile_definitions(media PRIVATE FW64_MEDIA_DIR_NAME="${media_dir_name}")

    #TODO: this should be moved out once the n64_libultra container has node enabled
    add_custom_command(TARGET media POST_BUILD
        COMMAND node ${CMAKE_CURRENT_SOURCE_DIR}/PrepareMediaDir.js ${FW64_PLATFORM_NAME} ${media_dir_name}
    )
endif()
