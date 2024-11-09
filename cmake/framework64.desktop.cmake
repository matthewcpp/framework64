set(FW64_PLATFORM_DESKTOP ON)
set(CMAKE_CXX_STANDARD 17)

function (enable_all_warnings_as_errors)
    set(options)
    set(oneValueArgs TARGET)
    set(multiValueArgs)
    cmake_parse_arguments(ENABLE_ALL_WARNINGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

    set(target_name ${ENABLE_ALL_WARNINGS_TARGET})

    set_property(TARGET ${target_name} PROPERTY COMPILE_WARNING_AS_ERROR ON)

    if (MSVC)
        # warning level 4
        target_compile_options(${target_name} PRIVATE /W4)
        target_compile_definitions(${target_name} PRIVATE _CRT_SECURE_NO_WARNINGS)
    else()
        # additional warnings
        target_compile_options(${target_name} PRIVATE -Wall -Wextra -Wpedantic -Wno-strict-prototypes)
    endif()
endfunction()

# performs platform specific configuration of the framework 64 library
function (configure_core_library)
    if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
        message(FATAL_ERROR "Desktop build requires x64 architecture")
    endif()

    find_package(CLI11 CONFIG REQUIRED)
    find_package(GLEW REQUIRED)
    find_package(nlohmann_json CONFIG REQUIRED)
    find_package(SDL2 CONFIG REQUIRED)
    find_package(SDL2_mixer CONFIG REQUIRED)
    find_package(SDL2_image CONFIG REQUIRED)
    find_package(CURL CONFIG REQUIRED)
    find_package(websocketpp CONFIG REQUIRED)

    target_link_libraries(framework64 PUBLIC GLEW::GLEW)
    # mac SDL mixer?: SDL2_mixer::SDL2_mixer-static
    target_link_libraries(framework64 PUBLIC SDL2::SDL2 SDL2::SDL2main)
    target_link_libraries(framework64 PUBLIC $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>)
    target_link_libraries(framework64 PUBLIC $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>)
    target_link_libraries(framework64 PUBLIC CURL::libcurl)
    target_link_libraries(framework64 PUBLIC websocketpp::websocketpp)
    target_link_libraries(framework64 PUBLIC CLI11::CLI11)

    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)

    target_compile_definitions(framework64 PUBLIC FW64_PLATFORM_DESKTOP)
    target_compile_definitions(framework64 PUBLIC FW64_PLATFORM_IS_64_BIT)

    enable_all_warnings_as_errors(TARGET framework64)
endfunction()

# performs platform specific configuration of a framework64 game
function(create_game)
    set(options ALL_WARNINGS_AS_ERRORS)
    set(oneValueArgs TARGET SAVE_FILE_TYPE GAME_HEADER_PATH)
    set(multiValueArgs SOURCES EXTRA_LIBS)
    cmake_parse_arguments(DESKTOP_GAME "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

    set(target_name ${DESKTOP_GAME_TARGET})
    set(game_sources ${DESKTOP_GAME_SOURCES})

    if (DEFINED DESKTOP_GAME_GAME_HEADER_PATH)
        set(game_include_path ${DESKTOP_GAME_GAME_HEADER_PATH})
    else()
        set(game_include_path "game.h")
    endif()

    set(main_file_src ${FW64_ROOT_DIR}/src/framework64/desktop/main_desktop.cpp)
    set(main_file_dest ${CMAKE_CURRENT_BINARY_DIR}/main_desktop_${target_name}.cpp)
    configure_file(${main_file_src} ${main_file_dest})

    add_executable(${target_name} ${game_sources} ${main_file_dest})
    target_link_libraries(${target_name} PUBLIC framework64)

    if (${DESKTOP_GAME_ALL_WARNINGS_AS_ERRORS})
        enable_all_warnings_as_errors(TARGET ${target_name})
    endif()

    # Configure target specific output directories
    set(output_dir ${CMAKE_BINARY_DIR}/bin/${target_name})
    set_target_properties(${target_name} PROPERTIES 
        RUNTIME_OUTPUT_DIRECTORY ${output_dir}
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${output_dir}
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${output_dir})

    # include the game specific asset directory
    set(asset_include_dir ${output_dir}/assets/include)
    target_include_directories(${target_name} PUBLIC ${asset_include_dir})

    # Add any extra libraries that need to be linked in
    if (DEFINED DESKTOP_GAME_EXTRA_LIBS)
        target_link_libraries(${target_name} PUBLIC ${DESKTOP_GAME_EXTRA_LIBS})
    endif()

    # Fixes linker error encountered on linux
    # https://stackoverflow.com/a/55086637
    if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        target_link_options(${target_name} PUBLIC "-Wl,--copy-dt-needed-entries")
    endif()

    target_include_directories(${target_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

    if (NOT DEFINED DESKTOP_GAME_SAVE_FILE_TYPE)
        set(DESKTOP_GAME_SAVE_FILE_TYPE "NONE")
    endif()

    target_compile_definitions(${target_name} 
        PUBLIC FW64_APPLICATION_NAME="${target_name}"
        PUBLIC FW64_SAVE_FILE_TYPE="${DESKTOP_GAME_SAVE_FILE_TYPE}")

endfunction()