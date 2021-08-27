set(include_dir ../include/framework64)

find_package(GLEW REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(sdl2-mixer CONFIG REQUIRED)
find_package(sdl2-image CONFIG REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)

set(source_files
        ${include_dir}/box.h box.c
        ${include_dir}/camera.h camera.c
        ${include_dir}/color.h color.c
        ${include_dir}/frustum.h frustum.c
        ${include_dir}/matrix.h matrix.c
        ${include_dir}/plane.h plane.c
        ${include_dir}/quat.h quat.c
        ${include_dir}/sparse_set.h sparse_set.c
        ${include_dir}/transform.h transform.c
        ${include_dir}/types.h
        ${include_dir}/time.h time.c
        ${include_dir}/vec2.h vec2.c
        ${include_dir}/vec3.h vec3.c)

set(desktop_files
        ${include_dir}/assets.h ${include_dir}/desktop/assets.h desktop/assets.cpp
        ${include_dir}/audio.h ${include_dir}/desktop/audio.h desktop/audio.cpp
        ${include_dir}/engine.h ${include_dir}/desktop/engine.h desktop/engine.cpp
        ${include_dir}/filesystem.h ${include_dir}/desktop/filesystem.h desktop/filesystem.cpp
        ${include_dir}/input.h ${include_dir}/desktop/input.h desktop/input.cpp
        ${include_dir}/renderer.h ${include_dir}/desktop/renderer.h desktop/renderer.cpp
        ${include_dir}/desktop/shader.h desktop/shader.cpp
        ${include_dir}/desktop/sprite_renderer.h desktop/sprite_renderer.cpp
        ${include_dir}/texture.h ${include_dir}/desktop/texture.h desktop/texture.cpp
        )

add_library(framework64 STATIC ${source_files} ${desktop_files})

target_include_directories(framework64 PUBLIC ../include ../assets/build_desktop)

target_link_libraries(framework64 PRIVATE GLEW::GLEW)
target_link_libraries(framework64 INTERFACE SDL2::SDL2 SDL2::SDL2main)
target_link_libraries(framework64 PRIVATE SDL2::SDL2_image)
target_link_libraries(framework64 PRIVATE unofficial::sqlite3::sqlite3)