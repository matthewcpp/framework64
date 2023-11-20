add_subdirectory(data_link_asset_viewer)

if (FW64_PLATFORM_DESKTOP)
    add_subdirectory(data_link_client)
    add_subdirectory(asset_transfer_webservice)
endif()
