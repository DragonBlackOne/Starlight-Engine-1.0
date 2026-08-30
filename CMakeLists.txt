cmake_minimum_required(VERSION 3.20)
project(StarlightEngineWorkspace VERSION 11.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

# Dependency root resolution
if(DEFINED STARLIGHT_DEPS_DIR)
    set(STARLIGHT_DEPS_DIR "${STARLIGHT_DEPS_DIR}" CACHE PATH "Dependency root directory")
elseif(DEFINED ENV{STARLIGHT_DEPS_DIR})
    set(STARLIGHT_DEPS_DIR "$ENV{STARLIGHT_DEPS_DIR}" CACHE PATH "Dependency root directory")
else()
    set(STARLIGHT_DEPS_DIR "d:/deps" CACHE PATH "Dependency root directory")
endif()
message(STATUS "Workspace: using STARLIGHT_DEPS_DIR = ${STARLIGHT_DEPS_DIR}")

# Global configuration flags (without global /W4 /WX which breaks third-party libraries)
if(MSVC)
    add_compile_options(/MP /EHsc /FS /utf-8 /bigobj)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS GLM_ENABLE_EXPERIMENTAL SOL_USING_CXX_LUA)
endif()

# Build Tests option
option(STARLIGHT_BUILD_TESTS "Build unit tests with GoogleTest" ON)
if(STARLIGHT_BUILD_TESTS)
    enable_testing()
endif()

# Add Starlight Engine target (static lib + showcase)
add_subdirectory(StarlightEngine)

# Fusion Game helper (add_fusion_game)
include(cmake/FusionGameHelpers.cmake)

# Add all Game projects
add_subdirectory(CapitalOdyssey)
add_subdirectory(FusionFight)
add_subdirectory(GodHand_Project)
add_subdirectory(Pong_Project)
add_subdirectory(Snake_Project)
add_subdirectory(Tetris_Project)
add_subdirectory(TechDemo_Project)
