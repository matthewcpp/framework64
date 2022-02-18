set(desktop_include_directory ${CMAKE_CURRENT_SOURCE_DIR}/../../include/framework64/desktop)

set(platform_source_files

${desktop_include_directory}/asset_database.h desktop/asset_database.cpp
${desktop_include_directory}/audio.h desktop/audio.cpp
${desktop_include_directory}/audio_bank.h desktop/audio_bank.cpp
${desktop_include_directory}/collider.h
${desktop_include_directory}/engine.h desktop/engine.cpp
${desktop_include_directory}/filesystem.h desktop/filesystem.cpp
${desktop_include_directory}/font.h desktop/font.cpp
${desktop_include_directory}/glb_parser.h desktop/glb_parser.cpp
${desktop_include_directory}/gouraud_shader.h desktop/gouraud_shader.cpp
${desktop_include_directory}/input.h desktop/input.cpp
${desktop_include_directory}/image.h desktop/image.cpp
${desktop_include_directory}/input_interface.h
${desktop_include_directory}/json_map.h desktop/json_map.cpp
${desktop_include_directory}/line_shader.h desktop/line_shader.cpp
${desktop_include_directory}/material.h desktop/material.cpp
${desktop_include_directory}/mesh.h desktop/mesh.cpp
${desktop_include_directory}/mesh_data.h desktop/mesh_data.cpp
${desktop_include_directory}/n64_input_interface.h desktop/n64_input_interface.cpp
${desktop_include_directory}/pixel_texture.h desktop/pixel_texture.cpp
desktop/quad.cpp
${desktop_include_directory}/renderer.h desktop/renderer.cpp
desktop/render_util.cpp
${desktop_include_directory}/scene.h desktop/scene.cpp
${desktop_include_directory}/screen_overlay.h desktop/screen_overlay.cpp
${desktop_include_directory}/shader.h desktop/shader.cpp
${desktop_include_directory}/shader_cache.h desktop/shader_cache.cpp
${desktop_include_directory}/splitter.h desktop/splitter.cpp
${desktop_include_directory}/sprite_batch.h desktop/sprite_batch.cpp
${desktop_include_directory}/sprite_shader.h desktop/sprite_shader.cpp
${desktop_include_directory}/texture.h desktop/texture.cpp
${desktop_include_directory}/texture_info_uniform.h
${desktop_include_directory}/uniform_block.h desktop/uniform_block.cpp
${desktop_include_directory}/unlit_shader.h desktop/unlit_shader.cpp

${CMAKE_CURRENT_SOURCE_DIR}/../../include/framework64/n64/controller_button.h

PARENT_SCOPE
)