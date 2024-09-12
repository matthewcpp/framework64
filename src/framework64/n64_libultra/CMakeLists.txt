set(platform_source_files
    asset_database.h asset_database.c
    audio.h audio.c
    compat.h compat.c
    data_link.h data_link.c
    display.h display.c
    display_list.h display_list.c
    engine.h engine.c
    filesystem.h filesystem.c
    font.h font.c
    image.h image.c
    input.h input.c
    material.h material.c
    material_bundle.h material_bundle.c
    media.h media.c
    mesh.h mesh.c
    mesh_builder.h mesh_builder.c
    mesh_data_itr.c
    mesh_editor.c
    render_pass.h render_pass.c
    renderer.h renderer.c
    renderer_util.c
    save_file.h save_file.c
    sprite_batch.h sprite_batch.c
    texture.h texture.c
    texture_batch.h texture_batch.c
    vertex.h vertex.c

    usb/usb.h usb/usb.c
)

target_sources(framework64 PRIVATE ${platform_source_files})