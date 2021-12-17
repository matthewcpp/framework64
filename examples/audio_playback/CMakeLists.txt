set(src_files
    game.h game.c)

create_game(
    TARGET audio_playback
    SOURCES ${src_files})