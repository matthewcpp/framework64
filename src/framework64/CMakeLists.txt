set(common_source_files
    allocator.h allocator.c
    animation_controller.h animation_controller.c
    animation_data.h animation_data.c
    asset_database.h
    audio.h
    box.h box.c
    camera.h camera.c
    collision.h collision.c
    collision_mesh.h collision_mesh.c
    collider.h collider.c
    color.h color.c
    data_io.h data_io.c
    data_link.h
    display.h
    engine.h
    filesystem.h
    font.h
    frustum.h frustum.c
    image.h
    input.h
    material.h
    material_bundle.h
    material_collection.h material_collection.c
    math.h math.c
    matrix.h matrix.c
    matrix_project.h matrix_project.c
    media.h
    mesh.h
    mesh_instance.h mesh_instance.c
    node.h node.c
    plane.h plane.c
    quat.h quat.c
    query.h query.c
    random.h random.c
    rect.h rect.c
    render_pass.h
    render_queue.h render_queue.c
    renderer.h
    scene.h scene.c
    skinned_mesh.h skinned_mesh.c
    skinned_mesh_instance.h skinned_mesh_instance.c
    sprite_batch.h
    static_vector.h static_vector.c
    texture.h
    transform.h transform.c
    types.h
    time.h
    vec2.h vec2.c
    vec3.h vec3.c
    vec4.h vec4.c
    viewport.h viewport.c

    util/bump_allocator.h util/bump_allocator.c
    util/dynamic_vector.h util/dynamic_vector.c
    util/mesh_builder.h
    util/mesh_data_itr.h
    util/mesh_editor.h
    util/quad.h util/quad.c
    util/renderer_util.h
    util/renderpass_util.h util/renderpass_util.c
    util/sparse_set.h util/sparse_set.c
    util/text_util.h util/text_util.c
    util/texture_util.h util/texture_util.c

    controller_mapping/n64.h
)


add_library(framework64 STATIC ${common_source_files})
target_include_directories(framework64 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

if (FW64_PLATFORM_DESKTOP)
    add_subdirectory(desktop)
elseif(FW64_PLATFORM_N64_LIBULTRA)
    add_subdirectory(n64_libultra)
endif()

configure_core_library()

if (FW64_MEDIA_ENABLED)
    target_compile_definitions(framework64 PRIVATE -DFW64_MEDIA_ENABLED)
    
    if(FW64_PLATFORM_N64_LIBULTRA)
        include(n64_libultra/libcart.cmake)
    endif()

else()
    message("Building without media support")
endif()