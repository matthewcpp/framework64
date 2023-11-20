set(desktop_include_directory ${CMAKE_CURRENT_SOURCE_DIR}/../../include/framework64/desktop)

set(platform_source_files

${desktop_include_directory}/asset_database.h asset_database.cpp
${desktop_include_directory}/audio.h audio.cpp
${desktop_include_directory}/data_link.h data_link.cpp
${desktop_include_directory}/engine.h engine.cpp
${desktop_include_directory}/filesystem.h filesystem.cpp
${desktop_include_directory}/file_datasource.h file_datasource.cpp
${desktop_include_directory}/fill_shader.h fill_shader.cpp
${desktop_include_directory}/font.h font.cpp
${desktop_include_directory}/gouraud_shader.h gouraud_shader.cpp
${desktop_include_directory}/input.h input.cpp
${desktop_include_directory}/image.h image.cpp
${desktop_include_directory}/input_interface.h
${desktop_include_directory}/line_shader.h line_shader.cpp
log.cpp
${desktop_include_directory}/material.h material.cpp
${desktop_include_directory}/material_bundle.h material_bundle.cpp
${desktop_include_directory}/media.h media.cpp
${desktop_include_directory}/mesh.h mesh.cpp
mesh_data_itr.cpp
${desktop_include_directory}/n64_input_interface.h n64_input_interface.cpp
${desktop_include_directory}/primitive_data.h primitive_data.cpp
quad.cpp
${desktop_include_directory}/renderer.h renderer.cpp
render_util.cpp
${desktop_include_directory}/save_file.h save_file.cpp
${desktop_include_directory}/screen_overlay.h screen_overlay.cpp
${desktop_include_directory}/shader.h shader.cpp
${desktop_include_directory}/shader_cache.h shader_cache.cpp
${desktop_include_directory}/sprite_batch.h sprite_batch.cpp
${desktop_include_directory}/sprite_shader.h sprite_shader.cpp
${desktop_include_directory}/texture.h texture.cpp
${desktop_include_directory}/texture_info_uniform.h
${desktop_include_directory}/uniform_block.h uniform_block.cpp
${desktop_include_directory}/unlit_shader.h unlit_shader.cpp

${CMAKE_CURRENT_SOURCE_DIR}/../../include/framework64/n64/controller_button.h
)

target_sources(framework64 PRIVATE ${platform_source_files})

if (FW64_MEDIA_ENABLED)
    target_compile_definitions(framework64 PRIVATE -DFW64_MEDIA_ENABLED)
else()
    message("Building without media support")
endif()