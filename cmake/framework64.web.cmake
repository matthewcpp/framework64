set(FW64_PLATFORM_WEB ON)
set(CMAKE_CXX_STANDARD 17)

set(FW64_PLATFORM_NAME "web")

function (enable_all_warnings_as_errors)
    set(options)
    set(oneValueArgs TARGET)
    set(multiValueArgs)
    cmake_parse_arguments(ENABLE_ALL_WARNINGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

    set(target_name ${ENABLE_ALL_WARNINGS_TARGET})
    set_property(TARGET ${target_name} PROPERTY COMPILE_WARNING_AS_ERROR ON)
    target_compile_options(${target_name} PRIVATE -Wall -Wextra -Wpedantic -Wno-strict-prototypes)
endfunction()

# performs platform specific configuration of the framework 64 library
function (configure_core_library)
    target_compile_definitions(framework64 PUBLIC FW64_PLATFORM_WEB)
    #target_compile_options(framework64 PUBLIC -g)
    target_compile_options(framework64 PRIVATE "SHELL:-sUSE_SDL=2")
    target_compile_options(framework64 PUBLIC "SHELL:-sUSE_SDL_MIXER=2")
    target_compile_options(framework64 PRIVATE "SHELL:-sUSE_SDL_IMAGE=2")
    target_compile_options(framework64 PRIVATE "SHELL:-sSDL2_IMAGE_FORMATS=['png']")
endfunction()

# performs platform specific configuration of a framework64 game
function(create_game)
    set(options ALL_WARNINGS_AS_ERRORS)
    set(oneValueArgs TARGET SAVE_FILE_TYPE GAME_HEADER_PATH HTML_TEMPLATE_PATH)
    set(multiValueArgs SOURCES EXTRA_LIBS STATIC_MODULES)
    cmake_parse_arguments(WEB_GAME "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

    set(target_name ${WEB_GAME_TARGET})
    set(game_sources ${WEB_GAME_SOURCES})

    if (DEFINED WEB_GAME_GAME_HEADER_PATH)
        set(game_include_path ${WEB_GAME_GAME_HEADER_PATH})
    else()
        set(game_include_path "game.h")
    endif()

    set(main_file_src ${FW64_ROOT_DIR}/src/framework64/web/main_web.cpp)
    set(main_file_dest ${CMAKE_CURRENT_BINARY_DIR}/main_web_${target_name}.cpp)
    configure_file(${main_file_src} ${main_file_dest})

    add_executable(${target_name} ${game_sources} ${main_file_dest})
    set_target_properties(${target_name} PROPERTIES SUFFIX ".html")

    target_link_libraries(${target_name} PUBLIC framework64)

    add_game_modules(TARGET ${target_name} MODULES ${DESKTOP_GAME_STATIC_MODULES})

    if (${WEB_GAME_ALL_WARNINGS_AS_ERRORS})
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

    # bundle the assets and shaders into the binary
    target_link_options(${target_name} PUBLIC "SHELL:--embed-file ${output_dir}/assets@/assets")
    target_link_options(${target_name} PUBLIC "SHELL:--embed-file ${output_dir}/glsl@/glsl")
    target_link_options(${target_name} PUBLIC "SHELL:-sMAX_WEBGL_VERSION=2")
    target_link_options(${target_name} PUBLIC "SHELL:-sMIN_WEBGL_VERSION=2")
    target_link_options(${target_name} PUBLIC "SHELL:-sUSE_SDL=2")
    target_link_options(${target_name} PUBLIC "SHELL:-sUSE_SDL_MIXER=2")
    target_link_options(${target_name} PUBLIC "SHELL:-sUSE_SDL_IMAGE=2")
    target_link_options(${target_name} PUBLIC "SHELL:-sSDL2_IMAGE_FORMATS=['png']")

    if (DEFINED WEB_GAME_HTML_TEMPLATE_PATH)
        message("${target_name}: Using HTML template path: ${WEB_GAME_HTML_TEMPLATE_PATH}")
        target_link_options(${target_name} PRIVATE --shell-file ${WEB_GAME_HTML_TEMPLATE_PATH})
        target_link_options(${target_name} PRIVATE "SHELL:-sINVOKE_RUN=0")
        target_link_options(${target_name} PRIVATE "SHELL:-sEXPORTED_RUNTIME_METHODS=['callMain']")
    endif()

    # target_link_options(${target_name} PUBLIC "SHELL:-sEXPORTED_FUNCTIONS=['_main','requestFullscreen']")

    # Add any extra libraries that need to be linked in
    if (DEFINED WEB_GAME_EXTRA_LIBS)
        target_link_libraries(${target_name} PUBLIC ${WEB_GAME_EXTRA_LIBS})
    endif()

    target_include_directories(${target_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

    if (NOT DEFINED WEB_GAME_SAVE_FILE_TYPE)
        set(WEB_GAME_SAVE_FILE_TYPE "NONE")
    endif()

    target_compile_definitions(${target_name} 
        PUBLIC FW64_APPLICATION_NAME="${target_name}"
        PUBLIC FW64_SAVE_FILE_TYPE="${WEB_GAME_SAVE_FILE_TYPE}"
    )
endfunction()