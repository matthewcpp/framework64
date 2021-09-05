set(src_files
        ../common/main_desktop.cpp
        ../common/entity.h ../common/entity.c
        game.h game.c)

add_executable(rendermodes ${src_files})

target_link_libraries(rendermodes framework64)
target_include_directories(rendermodes PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../common)