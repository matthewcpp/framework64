set(libdragon_source_files
    libdragon_asset_database.h libdragon_asset_database.c
    libdragon_display.h libdragon_display.c
    libdragon_engine.h libdragon_engine.c
    libdragon_image.h libdragon_image.c
    libdragon_input.h libdragon_input.c
    libdragon_renderer.h libdragon_renderer.c
    libdragon_render_pass.h libdragon_render_pass.c
    libdragon_sprite_batch.h libdragon_sprite_batch.c
    libdragon_texture.h libdragon_texture.c
)

target_sources(framework64 PRIVATE ${libdragon_source_files})