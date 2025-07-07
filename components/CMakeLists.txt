set(components_source_files
    fw64_arcball_camera.h fw64_arcball_camera.c
    fw64_asset_viewer.h fw64_asset_viewer.c
    fw64_billboard_node.h fw64_billboard_node.c
    fw64_debug_boxes.h fw64_debug_boxes.c
    fw64_file_downloader_message.h fw64_file_downloader.h fw64_file_downloader.c
    fw64_fps_camera.h fw64_fps_camera.c
    fw64_headlight.h fw64_headlight.c
    fw64_media_file_picker.h fw64_media_file_picker.c
    fw64_mesh_prim_highlighter.h fw64_mesh_prim_highlighter.c
    fw64_rotate_node.h fw64_rotate_node.c
    fw64_transform_editor.h fw64_transform_editor.c
    fw64_ui_navigation.h fw64_ui_navigation.c
    fw64_wireframe.h fw64_wireframe.c
)

add_library(fw64_components STATIC ${components_source_files})

enable_all_warnings_as_errors(TARGET fw64_components)
add_game_modules(TARGET fw64_components MODULES media)

target_include_directories(fw64_components PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(fw64_components PUBLIC framework64)