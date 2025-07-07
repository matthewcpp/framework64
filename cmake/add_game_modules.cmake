function (add_game_modules)
    set(options)
    set(oneValueArgs TARGET)
    set(multiValueArgs MODULES)
    cmake_parse_arguments(GAME "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

foreach(module ${GAME_MODULES})
    # message("Enabling module for target ${GAME_TARGET}: ${module}")
    target_link_libraries(${GAME_TARGET} PRIVATE "framework64_${module}")
endforeach()

endfunction()