set(components_source_files
    arcball_camera/arcball_camera.h arcball_camera/arcball_camera.c
    asset_viewer/asset_viewer.h asset_viewer/asset_viewer.c
    file_explorer/file_explorer.h file_explorer/file_explorer.c
    file_downloader/file_downloader_message.h file_downloader/file_downloader.h file_downloader/file_downloader.c
    fps_camera/fps_camera.h fps_camera/fps_camera.c
    )

add_library(fw64_components STATIC ${components_source_files})

target_include_directories(fw64_components PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(fw64_components PUBLIC framework64)