cmake_minimum_required(VERSION 3.20)
project(Snake_Project LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

include("${CMAKE_CURRENT_SOURCE_DIR}/../StarlightEngine/cmake/StarlightProject.cmake")
add_starlight_game(Snake_Project MODE_2D)
