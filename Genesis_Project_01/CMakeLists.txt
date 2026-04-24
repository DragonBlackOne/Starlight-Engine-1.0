cmake_minimum_required(VERSION 3.20)
project(Genesis_Project_01)

set(CMAKE_CXX_STANDARD 20)

# Localizar o SDK do Starlight Engine
set(STARLIGHT_SDK_DIR "D:/Projetos/Fusion ENGINE/StarlightEngine")
include(${STARLIGHT_SDK_DIR}/StarlightEngineConfig.cmake)

add_executable(Genesis_Project_01 src/main.cpp)
target_link_libraries(Genesis_Project_01 PRIVATE StarlightCore)

# Copiar Assets para o build
add_custom_command(TARGET Genesis_Project_01 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:Genesis_Project_01>/assets
)
