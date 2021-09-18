set(src_files
        ../common/main_desktop.cpp
        ../common/entity.h ../common/entity.c
        game.h game.c)

add_executable(billboard ${src_files})

target_link_libraries(billboard framework64)
target_include_directories(billboard PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../common)