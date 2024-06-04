
find_package(tiny-process-library CONFIG REQUIRED)
find_package(stduuid CONFIG REQUIRED)
find_package(CLI11 CONFIG REQUIRED)

find_package(unofficial-http-parser CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(restinio CONFIG REQUIRED)

set(data_link_asset_service_src
    asset_pipeline.hpp asset_pipeline.cpp
    asset_process.hpp asset_process.cpp
    file_transfer.hpp file_transfer.cpp
    settings.hpp
    temp_dir.hpp
    web_service.hpp web_service.cpp

    main.cpp
)

add_executable(data_link_asset_service ${data_link_asset_service_src})

enable_all_warnings_as_errors(TARGET data_link_asset_service)

target_include_directories(data_link_asset_service PRIVATE ../../components/file_downloader)

target_link_libraries(data_link_asset_service PRIVATE fw64_datalink_client)
target_link_libraries(data_link_asset_service PRIVATE restinio::restinio)
target_link_libraries(data_link_asset_service PRIVATE tiny-process-library::tiny-process-library)
target_link_libraries(data_link_asset_service PRIVATE stduuid)
target_link_libraries(data_link_asset_service PRIVATE CLI11::CLI11)

# Needed for stduuid
if (APPLE)
    find_library(CFLIB CoreFoundation REQUIRED)
    target_link_libraries(data_link_asset_service PRIVATE ${CFLIB})
endif()