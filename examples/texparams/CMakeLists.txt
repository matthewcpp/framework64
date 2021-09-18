set(src_files
        ../common/main_desktop.cpp
        ../common/entity.h ../common/entity.c
        game.h game.c)

add_executable(texparams ${src_files})

target_link_libraries(texparams framework64)
target_include_directories(texparams PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})