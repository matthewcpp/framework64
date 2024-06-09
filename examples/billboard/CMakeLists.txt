set(src_files
    ../common/common/billboard_node.h ../common/common/billboard_node.c

    flame.h flame.c
    game.h game.c)

create_game(
    TARGET billboard
    SOURCES ${src_files}
    EXTRA_LIBS fw64_components
    ALL_WARNINGS_AS_ERRORS)

target_include_directories(billboard PRIVATE ../common)