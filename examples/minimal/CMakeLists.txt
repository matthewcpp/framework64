set(src_files
        ../common/main_desktop.cpp
        game.h game.c)

add_executable(minimal ${src_files})

target_link_libraries(minimal framework64)
target_include_directories(minimal PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})