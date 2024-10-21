
set(platform_source_files
    asset_database.hpp asset_database.cpp
    audio.hpp audio.cpp
    data_link.hpp data_link.cpp
    display.hpp display.cpp
    engine.hpp engine.cpp
    filesystem.hpp filesystem.cpp
    file_datasource.hpp file_datasource.cpp
    fmv.hpp fmv.cpp
    font.hpp font.cpp
    framebuffer.hpp framebuffer.cpp
    gouraud_shader.hpp gouraud_shader.cpp
    input.hpp input.cpp
    image.hpp image.cpp
    input_interface.hpp
    line_shader.hpp line_shader.cpp
    log.cpp
    material.hpp material.cpp
    material_bundle.hpp material_bundle.cpp
    media.hpp media.cpp
    mesh.hpp mesh.cpp
    mesh_builder.hpp mesh_builder.cpp
    mesh_data_itr.cpp
    mesh_editor.cpp
    n64_input_interface.hpp n64_input_interface.cpp
    primitive_data.hpp primitive_data.cpp
    render_pass.hpp render_pass.cpp
    renderer.hpp renderer.cpp
    save_file.hpp save_file.cpp
    settings.hpp
    shader.hpp shader.cpp
    shader_cache.hpp shader_cache.cpp
    sprite_batch.hpp sprite_batch.cpp
    sprite_shader.hpp sprite_shader.cpp
    sprite_vertex.hpp sprite_vertex.cpp
    texture.hpp texture.cpp
    texture_info_uniform.hpp
    uniform_block.hpp uniform_block.cpp
    unlit_shader.hpp unlit_shader.cpp
)

target_sources(framework64 PRIVATE ${platform_source_files})

if (FW64_MEDIA_ENABLED)
    target_compile_definitions(framework64 PRIVATE -DFW64_MEDIA_ENABLED)
else()
    message("Building without media support")
endif()