if(FW64_PLATFORM_DESKTOP)
    find_package(GLEW REQUIRED)
    find_package(nlohmann_json CONFIG REQUIRED)
    find_package(SDL2 CONFIG REQUIRED)
    find_package(sdl2-mixer CONFIG REQUIRED)
    find_package(sdl2-image CONFIG REQUIRED)
    find_package(unofficial-sqlite3 CONFIG REQUIRED)
endif()

set(common_source_files
        ../include/framework64/asset_database.h
        ../include/framework64/audio.h
        ../include/framework64/audio_bank.h
        ../include/framework64/box.h box.c
        ../include/framework64/camera.h camera.c
        ../include/framework64/color.h color.c
        ../include/framework64/engine.h
        ../include/framework64/filesystem.h
        ../include/framework64/font.h
        ../include/framework64/frustum.h frustum.c
        ../include/framework64/image.h
        ../include/framework64/input.h
        ../include/framework64/matrix.h matrix.c
        ../include/framework64/mesh.h
        ../include/framework64/plane.h plane.c
        ../include/framework64/quat.h quat.c
        ../include/framework64/renderer.h
        ../include/framework64/sparse_set.h sparse_set.c
        ../include/framework64/texture.h
        ../include/framework64/transform.h transform.c
        ../include/framework64/types.h
        ../include/framework64/time.h
        ../include/framework64/vec2.h vec2.c
        ../include/framework64/vec3.h vec3.c

        ../include/framework64/util/arcball_camera.h util/arcball_camera.c
        ../include/framework64/util/fps_camera.h util/fps_camera.c
        ../include/framework64/util/quad.h
        ../include/framework64/util/terrain.h util/terrain.c
        )

if (FW64_PLATFORM_N64)
    set(platform_source_files
            ../include/framework64/n64/asset_database.h
            ../include/framework64/n64/audio.h n64/audio.c
            ../include/framework64/n64/audio_bank.h n64/audio_bank.c
            ../include/framework64/n64/controller_button.h
            ../include/framework64/n64/engine.h n64/engine.c
            ../include/framework64/n64/filesystem.h n64/filesystem.c
            ../include/framework64/n64/font n64/font.c
            ../include/framework64/n64/image.h n64/image.c
            ../include/framework64/n64/input.h n64/input.c
            ../include/framework64/n64/loader.h n64/loader.c
            ../include/framework64/n64/material.h n64/material.c
            ../include/framework64/n64/mesh.h n64/mesh.c
            n64/quad.c
            ../include/framework64/n64/renderer.h n64/renderer.c
            n64/terrain.c
            ../include/framework64/n64/texture.h n64/texture.c
        )
else()
    set(platform_source_files
        ../include/framework64/desktop/asset_database.h desktop/asset_database.cpp
        ../include/framework64/desktop/audio.h desktop/audio.cpp
        ../include/framework64/desktop/audio_bank.h desktop/audio_bank.cpp
        ../include/framework64/desktop/engine.h desktop/engine.cpp
        ../include/framework64/desktop/filesystem.h desktop/filesystem.cpp
        ../include/framework64/desktop/font.h desktop/font.cpp
        ../include/framework64/desktop/glb_parser.h desktop/glb_parser.cpp
        ../include/framework64/desktop/gouraud_shader.h desktop/gouraud_shader.cpp
        ../include/framework64/desktop/input.h desktop/input.cpp
        ../include/framework64/desktop/image.h desktop/image.cpp
        ../include/framework64/desktop/input_interface.h
        ../include/framework64/desktop/line_shader.h desktop/line_shader.cpp
        ../include/framework64/desktop/material.h desktop/material.cpp
        ../include/framework64/desktop/mesh.h desktop/mesh.cpp
        ../include/framework64/desktop/mesh_data.h desktop/mesh_data.cpp
        ../include/framework64/desktop/mesh_renderer.h desktop/mesh_renderer.cpp
        ../include/framework64/desktop/n64_input_interface.h desktop/n64_input_interface.cpp
        ../include/framework64/desktop/particle_shader.h desktop/particle_shader.cpp
        desktop/quad.cpp
        ../include/framework64/desktop/renderer.h desktop/renderer.cpp
        ../include/framework64/desktop/shader.h desktop/shader.cpp
        ../include/framework64/desktop/shader_cache.h desktop/shader_cache.cpp
        ../include/framework64/desktop/sprite_renderer.h desktop/sprite_renderer.cpp
        desktop/terrain.cpp
        ../include/framework64/desktop/texture.h desktop/texture.cpp
        ../include/framework64/desktop/uniform_block.h
        ../include/framework64/desktop/vertex_color_shader.h desktop/vertex_color_shader.cpp
        ../include/framework64/n64/controller_button.h
        )
endif()

add_library(framework64 STATIC ${common_source_files} ${platform_source_files})
target_include_directories(framework64 PUBLIC ../include)

if(FW64_PLATFORM_DESKTOP)
    target_link_libraries(framework64 PRIVATE GLEW::GLEW)
    target_link_libraries(framework64 INTERFACE SDL2::SDL2 SDL2::SDL2main SDL2::SDL2_image SDL2::SDL2_mixer)
    target_link_libraries(framework64 PRIVATE unofficial::sqlite3::sqlite3)
endif()