set(n64_libultra_source_files
    asset_bundle_reader.h asset_bundle_reader.c
    asset_database.h asset_database.c
    engine.h engine.c
    image.h image.c
    material.h material.c
    material_bundle.h material_bundle.c
    mesh.h mesh.c
    texture.h texture.c
    renderer.h renderer.c
    asset_bundle_reader.h asset_bundle_reader.c
)

target_sources(framework64 PRIVATE ${n64_libultra_source_files})