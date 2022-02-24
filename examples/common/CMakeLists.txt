set(common_source_files
    include/common/arcball_camera.h src/arcball_camera.c
    include/common/fps_camera.h src/fps_camera.c)

add_library(example_common STATIC ${common_source_files})

target_include_directories(example_common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(example_common PUBLIC framework64)