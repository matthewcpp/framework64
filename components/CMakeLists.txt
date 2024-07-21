set(components_source_files
    arcball_camera/arcball_camera.h arcball_camera/arcball_camera.c
    billboard_node/billboard_node.h billboard_node/billboard_node.c
    debug_boxes/debug_boxes.h debug_boxes/debug_boxes.c
    asset_viewer/asset_viewer.h asset_viewer/asset_viewer.c
    media_file_picker/media_file_picker.h media_file_picker/media_file_picker.c
    file_downloader/file_downloader_message.h file_downloader/file_downloader.h file_downloader/file_downloader.c
    fps_camera/fps_camera.h fps_camera/fps_camera.c
    rotate_node/rotate_node.h rotate_node/rotate_node.c
    ui_navigation/ui_navigation.h ui_navigation/ui_navigation.c
    )

add_library(fw64_components STATIC ${components_source_files})

enable_all_warnings_as_errors(TARGET fw64_components)

target_include_directories(fw64_components PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(fw64_components PUBLIC framework64)