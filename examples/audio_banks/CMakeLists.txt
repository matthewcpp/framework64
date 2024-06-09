set(src_files
    ../common/common/rotate_node.h ../common/common/rotate_node.c
    game.h game.c)

create_game(
    TARGET audio_banks
    SOURCES ${src_files}
    ALL_WARNINGS_AS_ERRORS)

target_include_directories(audio_banks PRIVATE ../common)