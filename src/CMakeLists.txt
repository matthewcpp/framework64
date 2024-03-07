set(common_source_files
        ../include/framework64/allocator.h allocator.c
        ../include/framework64/animation_controller.h animation_controller.c
        ../include/framework64/animation_data.h animation_data.c
        ../include/framework64/asset_database.h
        ../include/framework64/audio.h
        ../include/framework64/box.h box.c
        ../include/framework64/camera.h camera.c
        ../include/framework64/collision.h collision.c
        ../include/framework64/collider.h collider.c
        ../include/framework64/color.h color.c
        ../include/framework64/data_io.h data_io.c
        ../include/framework64/data_link.h
        ../include/framework64/display.h
        ../include/framework64/engine.h
        ../include/framework64/filesystem.h
        ../include/framework64/font.h
        ../include/framework64/frustum.h frustum.c
        ../include/framework64/image.h
        ../include/framework64/input.h
        ../include/framework64/level.h level.c
        ../include/framework64/material_bundle.h
        ../include/framework64/math.h math.c
        ../include/framework64/matrix.h matrix.c
        ../include/framework64/matrix_project.h matrix_project.c
        ../include/framework64/media.h
        ../include/framework64/mesh.h
        ../include/framework64/node.h node.c
        ../include/framework64/plane.h plane.c
        ../include/framework64/quat.h quat.c
        ../include/framework64/query.h query.c
        ../include/framework64/random.h random.c
        ../include/framework64/rect.h rect.c
        ../include/framework64/renderer.h
        ../include/framework64/scene.h scene.c
        ../include/framework64/skinned_mesh.h skinned_mesh.c
        ../include/framework64/texture.h
        ../include/framework64/transform.h transform.c
        ../include/framework64/types.h
        ../include/framework64/time.h
        ../include/framework64/vec2.h vec2.c
        ../include/framework64/vec3.h vec3.c
        ../include/framework64/vec4.h vec4.c

        ../include/framework64/util/bump_allocator.h util/bump_allocator.c
        ../include/framework64/util/mesh_builder.h
        ../include/framework64/util/mesh_data_itr.h
        ../include/framework64/util/mesh_editor.h
        ../include/framework64/util/quad.h util/quad.c
        ../include/framework64/util/renderer_util.h
        ../include/framework64/util/sparse_set.h util/sparse_set.c
        ../include/framework64/util/texture_util.h util/texture_util.c
        )


add_library(framework64 STATIC ${common_source_files})
target_include_directories(framework64 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)

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