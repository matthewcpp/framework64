set(platform_source_files
    asset_database.h asset_database.c
    audio.h audio.c
    compat.h compat.c
    display.h display.c
    display_list.h display_list.c
    engine.h engine.c
    filesystem.h filesystem.c
    font.h font.c
    image.h image.c
    input.h input.c
    log.h log.c
    material.h material.c
    material_bundle.h material_bundle.c
    mesh.h mesh.c
    mesh_builder.h mesh_builder.c
    mesh_data_itr.c
    mesh_editor.c
    modules.h modules.c
    render_pass.h render_pass.c
    renderer.h renderer.c
    save_file.h save_file.c
    sprite_batch.h sprite_batch.c
    texture.h texture.c
    texture_batch.h texture_batch.c
    vertex.h vertex.c
)

target_sources(framework64 PRIVATE ${platform_source_files})
