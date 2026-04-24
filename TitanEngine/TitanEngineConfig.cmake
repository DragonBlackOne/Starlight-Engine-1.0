# TitanEngineConfig.cmake - Fusion ENGINE SDK
# Este arquivo permite que projetos externos encontrem e usem o TitanEngine facilmente.

set(TITAN_ENGINE_FOUND TRUE)
set(TITAN_ENGINE_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/include")
set(TITAN_ENGINE_LIBRARIES TitanCore)

# Re-incluir as dependências do SDK
include("${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt")

message(STATUS "TitanEngine SDK Commercial v1.0.0 Loaded from ${CMAKE_CURRENT_LIST_DIR}")
