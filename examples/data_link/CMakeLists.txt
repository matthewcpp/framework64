set(src_files
    game.h game.c)

create_game(
    TARGET data_link
    SOURCES ${src_files}
    STATIC_MODULES datalink
    EXTRA_LIBS fw64_components
    ALL_WARNINGS_AS_ERRORS)
