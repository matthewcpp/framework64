set(src_files
    game.h game.c)

create_game(
    TARGET audio_banks
    SOURCES ${src_files}
    EXTRA_LIBS fw64_components
    ALL_WARNINGS_AS_ERRORS)

target_include_directories(audio_banks PRIVATE ../common)