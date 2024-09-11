set(desktop_include_directory ${CMAKE_CURRENT_SOURCE_DIR}/../../include/framework64/desktop)

set(platform_source_files

${desktop_include_directory}/asset_database.hpp asset_database.cpp
${desktop_include_directory}/audio.hpp audio.cpp
${desktop_include_directory}/data_link.hpp data_link.cpp
${desktop_include_directory}/display.hpp display.cpp
${desktop_include_directory}/engine.hpp engine.cpp
${desktop_include_directory}/filesystem.hpp filesystem.cpp
${desktop_include_directory}/file_datasource.hpp file_datasource.cpp
${desktop_include_directory}/font.hpp font.cpp
${desktop_include_directory}/framebuffer.hpp framebuffer.cpp
${desktop_include_directory}/gouraud_shader.hpp gouraud_shader.cpp
${desktop_include_directory}/input.hpp input.cpp
${desktop_include_directory}/image.hpp image.cpp
${desktop_include_directory}/input_interface.hpp
${desktop_include_directory}/line_shader.hpp line_shader.cpp
log.cpp
${desktop_include_directory}/material.hpp material.cpp
${desktop_include_directory}/material_bundle.hpp material_bundle.cpp
${desktop_include_directory}/media.hpp media.cpp
${desktop_include_directory}/mesh.hpp mesh.cpp
${desktop_include_directory}/mesh_builder.hpp mesh_builder.cpp
mesh_data_itr.cpp
mesh_editor.cpp
${desktop_include_directory}/n64_input_interface.hpp n64_input_interface.cpp
${desktop_include_directory}/primitive_data.hpp primitive_data.cpp
${desktop_include_directory}/render_pass.hpp render_pass.cpp
${desktop_include_directory}/renderer.hpp renderer.cpp
render_util.cpp
${desktop_include_directory}/save_file.hpp save_file.cpp
${desktop_include_directory}/settings.hpp
${desktop_include_directory}/screen_overlay.hpp screen_overlay.cpp
${desktop_include_directory}/shader.hpp shader.cpp
${desktop_include_directory}/shader_cache.hpp shader_cache.cpp
${desktop_include_directory}/sprite_batch.hpp sprite_batch.cpp
${desktop_include_directory}/sprite_shader.hpp sprite_shader.cpp
${desktop_include_directory}/sprite_vertex.hpp sprite_vertex.cpp
${desktop_include_directory}/texture.hpp texture.cpp
${desktop_include_directory}/texture_info_uniform.hpp
${desktop_include_directory}/uniform_block.hpp uniform_block.cpp
${desktop_include_directory}/unlit_shader.hpp unlit_shader.cpp
)

target_sources(framework64 PRIVATE ${platform_source_files})

if (FW64_MEDIA_ENABLED)
    target_compile_definitions(framework64 PRIVATE -DFW64_MEDIA_ENABLED)
else()
    message("Building without media support")
endif()