set(n64_libdragon_include_dir ${CMAKE_CURRENT_SOURCE_DIR}/../../include/framework64/n64_libdragon)

set(platform_source_files
    ${n64_libdragon_include_dir}/asset_bundle_reader.h asset_bundle_reader.c
    ${n64_libdragon_include_dir}/asset_database.h asset_database.c
    ${n64_libdragon_include_dir}/engine.h engine.c
    ${n64_libdragon_include_dir}/image.h image.c
    ${n64_libdragon_include_dir}/material.h material.c
    ${n64_libdragon_include_dir}/material_bundle.h material_bundle.c
    ${n64_libdragon_include_dir}/mesh.h mesh.c
    ${n64_libdragon_include_dir}/texture.h texture.c
    ${n64_libdragon_include_dir}/renderer.h renderer.c
    ${n64_libdragon_include_dir}/asset_bundle_reader.h asset_bundle_reader.c
)

target_sources(framework64 PRIVATE ${platform_source_files})