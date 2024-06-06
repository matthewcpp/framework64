set(src_files
    asset_viewer.h asset_viewer.c)

create_game(
    TARGET data_link_asset_viewer
    GAME_HEADER_PATH asset_viewer.h
    SOURCES ${src_files}
    EXTRA_LIBS fw64_components
    ALL_WARNINGS_AS_ERRORS)