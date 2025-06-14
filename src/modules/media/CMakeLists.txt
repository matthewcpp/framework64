set(fw64_src_dir ${CMAKE_CURRENT_SOURCE_DIR}/../../framework64)

add_library(framework64_media STATIC include/framework64/media.h)
target_include_directories(framework64_media PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(framework64_media PUBLIC framework64)
target_compile_definitions(framework64_datalink PUBLIC FW64_STATIC_MODULE_MEDIA_ENABLED)

if (FW64_PLATFORM_DESKTOP)
    set(desktop_dir ${fw64_src_dir}/desktop)
    target_sources(framework64_media PRIVATE ${desktop_dir}/media.hpp ${desktop_dir}/media.cpp)
elseif(FW64_PLATFORM_N64_LIBULTRA)
    set(n64_libultra_dir ${fw64_src_dir}/n64_libultra)
    target_sources(framework64_media PRIVATE ${n64_libultra_dir}/media.h ${n64_libultra_dir}/media.c)
    include(${n64_libultra_dir}/libcart.cmake)
else() #default implementation
    target_sources(framework64_media PRIVATE default.c)
endif()
