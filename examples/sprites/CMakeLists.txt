set(src_files
        ../common/main_desktop.cpp
        game.h game.c
        sprites.h sprites.c)

add_executable(sprites ${src_files})

target_link_libraries(sprites framework64)
target_include_directories(sprites PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})