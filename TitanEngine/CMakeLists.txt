cmake_minimum_required(VERSION 3.20)
project(TitanEngine VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(MSVC_RUNTIME "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
set(CMAKE_MSVC_RUNTIME_LIBRARY ${MSVC_RUNTIME})

# --- Open Source Dependencies (MIT License) via FetchContent ---
include(FetchContent)

# meshoptimizer (by zeux) - Vertex cache, overdraw & vertex fetch optimization
# License: MIT | Used by: Valve, AMD, Godot Engine
FetchContent_Declare(
    meshoptimizer
    GIT_REPOSITORY https://github.com/zeux/meshoptimizer.git
    GIT_TAG        v0.21
    GIT_SHALLOW    TRUE
)
set(MESHOPT_BUILD_DEMO OFF CACHE BOOL "" FORCE)
set(MESHOPT_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(MESHOPT_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(meshoptimizer)

# cgltf (by jkuhlmann) - Single-header glTF 2.0 parser
# License: MIT | Industry standard 3D format for PBR assets
FetchContent_Declare(
    cgltf
    GIT_REPOSITORY https://github.com/jkuhlmann/cgltf.git
    GIT_TAG        v1.14
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(cgltf)
add_library(cgltf INTERFACE)
target_include_directories(cgltf INTERFACE ${cgltf_SOURCE_DIR})

# --- Local Dependencies ---
set(DEPS_DIR "d:/deps")

# GLM
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE ${DEPS_DIR}/glm-1.0.1)

# EnTT
add_library(entt INTERFACE)
target_include_directories(entt INTERFACE ${DEPS_DIR}/entt-3.13.2/src)

# Sol2 & Lua
add_library(sol2 INTERFACE)
target_include_directories(sol2 INTERFACE ${DEPS_DIR}/sol2-3.3.0/include)

# Lua (Sources are in the root of the extracted zip from Lua repo)
file(GLOB LUA_SOURCES "${DEPS_DIR}/lua-5.4.6/*.c")
list(REMOVE_ITEM LUA_SOURCES 
    "${DEPS_DIR}/lua-5.4.6/lua.c" 
    "${DEPS_DIR}/lua-5.4.6/luac.c" 
    "${DEPS_DIR}/lua-5.4.6/onelua.c"
    "${DEPS_DIR}/lua-5.4.6/ltests.c"
)
add_library(lua STATIC ${LUA_SOURCES})
target_include_directories(lua PUBLIC ${DEPS_DIR}/lua-5.4.6)

# SDL2
add_subdirectory(${DEPS_DIR}/SDL-release-2.30.0 ${CMAKE_BINARY_DIR}/sdl2 EXCLUDE_FROM_ALL)

# Jolt Physics
set(USE_STATIC_MSVC_RUNTIME_LIBRARY OFF CACHE BOOL "" FORCE)
set(OVERRIDE_CXX_FLAGS OFF CACHE BOOL "" FORCE)
set(TARGET_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(TARGET_HELLO_WORLD OFF CACHE BOOL "" FORCE)
set(TARGET_SAMPLES OFF CACHE BOOL "" FORCE)
set(TARGET_VIEWER OFF CACHE BOOL "" FORCE)
add_subdirectory(${DEPS_DIR}/JoltPhysics-master/Build ${CMAKE_BINARY_DIR}/jolt EXCLUDE_FROM_ALL)

# --- TitanEngine App ---
set(TITAN_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(TITAN_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_library(glad STATIC ${TITAN_INCLUDE_DIR}/glad/src/glad.c)
target_include_directories(glad PUBLIC ${TITAN_INCLUDE_DIR}/glad/include)

file(GLOB_RECURSE CORE_SOURCES 
    "${TITAN_SRC_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/wicked/core/wiJobSystem.cpp"
)

file(GLOB IMGUI_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui/*.cpp"
)

add_executable(TitanEditor ${CORE_SOURCES} ${IMGUI_SOURCES} ${TITAN_SRC_DIR}/main.cpp)
target_include_directories(TitanEditor PRIVATE 
    ${TITAN_INCLUDE_DIR}
    ${TITAN_SRC_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/pl_mpeg
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/nlohmann
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/wicked/core
    ${DEPS_DIR}/JoltPhysics-master
)

target_link_libraries(TitanEditor PRIVATE 
    SDL2::SDL2main
    SDL2::SDL2-static
    glad
    Jolt
    glm
    entt
    sol2
    lua
    meshoptimizer
    cgltf
)

target_compile_definitions(TitanEditor PRIVATE SDL_MAIN_HANDLED UNICODE _UNICODE NOMINMAX _CRT_SECURE_NO_WARNINGS GLM_ENABLE_EXPERIMENTAL)
if(MSVC)
    target_compile_options(TitanEditor PRIVATE /W4 /EHsc /FS /utf-8 /wd5321)
else()
    target_compile_options(TitanEditor PRIVATE -Wall -Wextra -Wpedantic)
endif()
