set(n64_include_dir ${CMAKE_CURRENT_SOURCE_DIR}/../../include/framework64/n64)

set(platform_source_files
    ${n64_include_dir}/asset_database.h
    ${n64_include_dir}/audio.h n64/audio.c
    ${n64_include_dir}/audio_bank.h n64/audio_bank.c
    ${n64_include_dir}/compat.h n64/compat.c
    ${n64_include_dir}/controller_button.h
    ${n64_include_dir}/engine.h n64/engine.c
    ${n64_include_dir}/filesystem.h n64/filesystem.c
    ${n64_include_dir}/framebuffer.h n64/framebuffer.c
    ${n64_include_dir}/font n64/font.c
    ${n64_include_dir}/image.h n64/image.c
    ${n64_include_dir}/input.h n64/input.c
    ${n64_include_dir}/loader.h n64/loader.c
    ${n64_include_dir}/material.h n64/material.c
    ${n64_include_dir}/mesh.h n64/mesh.c
    n64/quad.c
    ${n64_include_dir}/renderer.h n64/renderer.c
    n64/renderer_util.c
    ${n64_include_dir}/scene.h n64/scene.c
    ${n64_include_dir}/texture.h n64/texture.c
    PARENT_SCOPE
)