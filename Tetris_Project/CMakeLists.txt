cmake_minimum_required(VERSION 3.20)
project(Tetris_Project)

set(CMAKE_CXX_STANDARD 20)

# Localizar o SDK do Starlight Engine
set(STARLIGHT_SDK_DIR "D:/Projetos/Fusion ENGINE/StarlightEngine")

# Adicionar a engine como subprojeto para herdar todos os targets e includes
add_subdirectory(${STARLIGHT_SDK_DIR} ${CMAKE_BINARY_DIR}/starlight EXCLUDE_FROM_ALL)

add_executable(Tetris_Project src/main.cpp)
target_link_libraries(Tetris_Project PRIVATE StarlightCore)

# Garantir acesso aos includes da engine e dependências (propagados pelo StarlightCore)
target_include_directories(Tetris_Project PRIVATE src)

# Copiar Assets para o build
add_custom_command(TARGET Tetris_Project POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:Tetris_Project>/assets
)
