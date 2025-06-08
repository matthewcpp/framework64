add_library(fw64_mouse_module STATIC fw64_mouse_module.h)
target_link_libraries(fw64_mouse_module PUBLIC framework64)
target_include_directories(fw64_mouse_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set(mouse_module_files
    platforms/default_mouse.c
)

if (FW64_PLATFORM_DESKTOP)
    set(mouse_module_files
        platforms/sdl_mouse.h platforms/sdl_mouse.cpp
    )

    find_package(SDL2 CONFIG REQUIRED)
    target_link_libraries(fw64_mouse_module PUBLIC SDL2::SDL2 )

endif()

target_sources(fw64_mouse_module PRIVATE ${mouse_module_files})