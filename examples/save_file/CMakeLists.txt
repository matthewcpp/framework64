set(src_files
    game.h game.c)

create_game(
    TARGET save_file
    SAVE_FILE_TYPE "N64_EEPROM_4K"
    SOURCES ${src_files}
    EXTRA_LIBS fw64_components
    ALL_WARNINGS_AS_ERRORS)