set(src_files
    game.h game.c)

create_game(
    TARGET web_html_template
    SOURCES ${src_files}
    HTML_TEMPLATE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/html_template.html
    ALL_WARNINGS_AS_ERRORS)

target_compile_definitions(web_html_template PRIVATE FW64_DESKTOP_CONTROLLER0_EMULATION=0)
