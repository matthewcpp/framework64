set(fw64_src_dir ${CMAKE_CURRENT_SOURCE_DIR}/../../framework64)

add_library(framework64_datalink STATIC include/framework64/data_link.h)
target_include_directories(framework64_datalink PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(framework64_datalink PUBLIC framework64)
target_compile_definitions(framework64_datalink PUBLIC FW64_STATIC_MODULE_DATA_LINK_ENABLED)

if (FW64_PLATFORM_DESKTOP)
    set(desktop_dir ${fw64_src_dir}/desktop)
    target_sources(framework64_datalink PRIVATE ${desktop_dir}/data_link.hpp ${desktop_dir}/data_link.cpp)

    find_package(ixwebsocket CONFIG REQUIRED)
    target_link_libraries(framework64_datalink PRIVATE ixwebsocket::ixwebsocket)
elseif(FW64_PLATFORM_N64_LIBULTRA)
    set(n64_libultra_dir ${fw64_src_dir}/n64_libultra)
    target_sources(framework64_datalink PRIVATE 
        ${n64_libultra_dir}/data_link.h ${n64_libultra_dir}/data_link.c
        ${n64_libultra_dir}/usb/usb.h ${n64_libultra_dir}/usb/usb.c)
else() #default implementation
    target_sources(framework64_datalink PRIVATE default.c)
endif()
