# Helper para registrar jogos na Fusion ENGINE sem duplicar código
function(add_fusion_game TARGET_NAME)
    cmake_parse_arguments(ARG "MODE_2D" "" "" ${ARGN})

    add_executable(${TARGET_NAME} src/main.cpp)

    target_link_libraries(${TARGET_NAME} PRIVATE StarlightCore)

    # Diretórios de inclusão do core
    target_include_directories(${TARGET_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/StarlightEngine/include
    )

    # Flags do compilador específicas do alvo
    if(MSVC)
        target_compile_options(${TARGET_NAME} PRIVATE /W4 /WX)
    else()
        target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra -Werror)
    endif()

    if(ARG_MODE_2D)
        target_compile_definitions(${TARGET_NAME} PRIVATE STARLIGHT_2D_ONLY)
    endif()

    # Copia automática de assets no pós-build (elimina dependência exclusiva do PowerShell)
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/StarlightEngine/assets/scripts"
            "$<TARGET_FILE_DIR:${TARGET_NAME}>/assets/scripts"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/StarlightEngine/assets/shaders"
            "$<TARGET_FILE_DIR:${TARGET_NAME}>/assets/shaders"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/StarlightEngine/assets/textures"
            "$<TARGET_FILE_DIR:${TARGET_NAME}>/assets/textures"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/StarlightEngine/assets/audio"
            "$<TARGET_FILE_DIR:${TARGET_NAME}>/assets/audio"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/StarlightEngine/assets/fonts"
            "$<TARGET_FILE_DIR:${TARGET_NAME}>/assets/fonts"
    )

    # Merge os assets ESPECÍFICOS do jogo por cima dos da engine
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/assets/scripts")
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_CURRENT_SOURCE_DIR}/assets/scripts"
                "$<TARGET_FILE_DIR:${TARGET_NAME}>/assets/scripts"
        )
    endif()
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/assets/textures")
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_CURRENT_SOURCE_DIR}/assets/textures"
                "$<TARGET_FILE_DIR:${TARGET_NAME}>/assets/textures"
        )
    endif()
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/assets/audio")
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_CURRENT_SOURCE_DIR}/assets/audio"
                "$<TARGET_FILE_DIR:${TARGET_NAME}>/assets/audio"
        )
    endif()
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/assets/fonts")
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_CURRENT_SOURCE_DIR}/assets/fonts"
                "$<TARGET_FILE_DIR:${TARGET_NAME}>/assets/fonts"
        )
    endif()

    # engine.ini de configuração: usa o do próprio jogo (se houver), senão o padrão do motor
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/assets/engine.ini")
        set(_INI_SRC "${CMAKE_CURRENT_SOURCE_DIR}/assets/engine.ini")
    else()
        set(_INI_SRC "${CMAKE_SOURCE_DIR}/StarlightEngine/assets/engine.ini")
    endif()
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${_INI_SRC}"
            "$<TARGET_FILE_DIR:${TARGET_NAME}>/assets/engine.ini"
    )
endfunction()
