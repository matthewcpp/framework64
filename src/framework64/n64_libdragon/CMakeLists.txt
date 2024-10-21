set(libdragon_source_files
    libdragon_audio.h libdragon_audio.c
    libdragon_asset_database.h libdragon_asset_database.c
    libdragon_display.h libdragon_display.c
    libdragon_dfs_datasource.h libdragon_dfs_datasource.c
    libdragon_engine.h libdragon_engine.c
    libdragon_fmv.h libdragon_fmv.c
    libdragon_image.h libdragon_image.c
    libdragon_input.h libdragon_input.c
    libdragon_material.h libdragon_material.c
    libdragon_material_bundle.h libdragon_material_bundle.c;
    libdragon_mesh.h libdragon_mesh.c
    libdragon_renderer.h libdragon_renderer.c
    libdragon_render_pass.h libdragon_render_pass.c
    libdragon_sprite_batch.h libdragon_sprite_batch.c
    libdragon_texture.h libdragon_texture.c
)

target_sources(framework64 PRIVATE ${libdragon_source_files})