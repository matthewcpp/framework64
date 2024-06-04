# Builds the Data link clinet library, maybe separate this out in the future?

# Base Client
set(base_client_sources
    include/data_link/client.hpp
    include/data_link/message.hpp 
    include/data_link/message_queue.hpp
    include/data_link/message_pool.hpp
)

if (NOT WIN32)
    set(FW64_BUILD_DATALINK_CLIENT_N64 OFF)
endif()

if(FW64_BUILD_DATALINK_CLIENT_N64)
    # N64 client
    set(n64_client_sources
        src/n64/UNFLoader/device.h src/n64/UNFLoader/device.cpp
        src/n64/UNFLoader/device_64drive.h src/n64/UNFLoader/device_64drive.cpp
        src/n64/UNFLoader/device_everdrive.h src/n64/UNFLoader/device_everdrive.cpp
        src/n64/UNFLoader/device_sc64.h src/n64/UNFLoader/device_sc64.cpp

        src/n64/UNFLoader/Include/ftd2xx.h src/n64/UNFLoader/Include/WinTypes.h

        include/data_link/n64_client.hpp src/n64/n64_client.cpp
    )
else()
    set(n64_client_sources include/data_link/n64_client.hpp src/n64/n64_client_stub.cpp)
endif()

add_library(fw64_datalink_client STATIC ${base_client_sources} ${n64_client_sources})
target_include_directories(fw64_datalink_client PRIVATE src src/n64/UNFLoader/)
target_compile_definitions(fw64_datalink_client PRIVATE DIABLE_PRAGMA_COMMENT_LINKING)

enable_all_warnings_as_errors(TARGET fw64_datalink_client)

if (WIN32)
    set(ftd_lib_dir ${CMAKE_CURRENT_SOURCE_DIR}/src/n64/UNFLoader/Include)

    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ftd_lib ftd2xx_x64.lib)
    else()
        set(ftd_lib ftd2xx.lib)
    endif()

    target_link_libraries(fw64_datalink_client PUBLIC shlwapi ${ftd_lib_dir}/${ftd_lib})
endif()

# Desktop Client

find_package(websocketpp CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS random)

set(desktop_client_sources
    include/data_link/desktop_client.hpp src/desktop/desktop_client.cpp
)

target_sources(fw64_datalink_client PRIVATE ${desktop_client_sources})
target_link_libraries(fw64_datalink_client PUBLIC websocketpp::websocketpp Boost::random)
target_include_directories(fw64_datalink_client PUBLIC include)

# Example Client

find_package(CLI11 CONFIG REQUIRED)

set(example_client_sources
    example/example_client.cpp
)

add_executable(data_link_example_client ${example_client_sources})
target_link_libraries(data_link_example_client PRIVATE fw64_datalink_client)

target_link_libraries(data_link_example_client PRIVATE CLI11::CLI11)