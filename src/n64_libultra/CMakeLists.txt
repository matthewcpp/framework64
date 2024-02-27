set(n64_include_dir ${CMAKE_CURRENT_SOURCE_DIR}/../../include/framework64/n64)

set(platform_source_files
    ${n64_include_dir}/asset_database.h asset_database.c
    ${n64_include_dir}/audio.h audio.c
    ${n64_include_dir}/compat.h compat.c
    ${n64_include_dir}/controller_button.h
    ${n64_include_dir}/data_link.h data_link.c
    ${n64_include_dir}/display.h display.c
    ${n64_include_dir}/engine.h engine.c
    ${n64_include_dir}/filesystem.h filesystem.c
    ${n64_include_dir}/fill_rect.h fill_rect.c
    ${n64_include_dir}/font.h font.c
    ${n64_include_dir}/image.h image.c
    ${n64_include_dir}/input.h input.c
    ${n64_include_dir}/material.h material.c
    ${n64_include_dir}/material_bundle.h material_bundle.c
    ${n64_include_dir}/media.h media.c
    ${n64_include_dir}/mesh.h mesh.c
    ${n64_include_dir}/mesh_builder.h mesh_builder.c
    mesh_data_itr.c
    mesh_editor.c
    ${n64_include_dir}/renderer.h renderer.c
    renderer_util.c
    ${n64_include_dir}/save_file.h save_file.c
    ${n64_include_dir}/texture.h texture.c
    ${n64_include_dir}/vertex.h vertex.c

    usb/usb.h usb/usb.c
)

target_sources(framework64 PRIVATE ${platform_source_files})