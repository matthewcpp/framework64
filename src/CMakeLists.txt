set(include_dir ../include/framework64)

set(source_files
        ${include_dir}/box.h box.c
        ${include_dir}/camera.h camera.c
        ${include_dir}/color.h color.c
        ${include_dir}/matrix.h matrix.c
        ${include_dir}/quat.h quat.c
        ${include_dir}/sparse_set.h sparse_set.c
        ${include_dir}/transform.h transform.c
        ${include_dir}/types.h
        ${include_dir}/time.h time.c
        ${include_dir}/vec2.h vec2.c
        ${include_dir}/vec3.h vec3.c)

add_library(framework64 STATIC ${source_files})
target_include_directories(framework64 PUBLIC ../include)