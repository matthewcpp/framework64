set(src_files
        ../common/main_desktop.cpp
        ../common/entity.h ../common/entity.c
        game.h game.c)

add_executable(lines ${src_files})

target_link_libraries(lines framework64)
target_include_directories(lines PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../common)