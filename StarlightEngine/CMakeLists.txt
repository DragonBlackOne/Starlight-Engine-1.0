cmake_minimum_required(VERSION 3.20)
project(StarlightEngine VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW) # Enable CMAKE_MSVC_RUNTIME_LIBRARY

set(STARLIGHT_SDK_ROOT ${CMAKE_CURRENT_LIST_DIR})

# --- Open Source Dependencies ---
include(FetchContent)
FetchContent_Declare(
    meshoptimizer
    GIT_REPOSITORY https://github.com/zeux/meshoptimizer.git
    GIT_TAG        v1.1
    GIT_SHALLOW    TRUE
)
set(MESHOPT_BUILD_DEMO OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(meshoptimizer)

FetchContent_Declare(
    cgltf
    GIT_REPOSITORY https://github.com/jkuhlmann/cgltf.git
    GIT_TAG        v1.15
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(cgltf)

FetchContent_Declare(
    ozz_animation
    GIT_REPOSITORY https://github.com/guillaumeblanc/ozz-animation.git
    GIT_TAG        0.16.0
    GIT_SHALLOW    TRUE
)
set(OZZ_ANIMATION_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(OZZ_ANIMATION_BUILD_HOWTOS OFF CACHE BOOL "" FORCE)
set(OZZ_ANIMATION_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ozz_build_msvc_rt_dll ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(ozz_animation)
add_library(cgltf INTERFACE)
target_include_directories(cgltf INTERFACE ${cgltf_SOURCE_DIR})

target_include_directories(cgltf INTERFACE ${cgltf_SOURCE_DIR})

set(DEPS_DIR "d:/deps")

# GLM, EnTT, Sol2, Lua
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE ${DEPS_DIR}/glm-1.0.3)
add_library(entt INTERFACE)
target_include_directories(entt INTERFACE ${DEPS_DIR}/entt-3.16.0/src)
add_library(sol2 INTERFACE)
target_include_directories(sol2 INTERFACE ${DEPS_DIR}/sol2-3.5.0/include)

file(GLOB LUA_SOURCES "${DEPS_DIR}/lua-5.4.7/*.c")
list(REMOVE_ITEM LUA_SOURCES 
    "${DEPS_DIR}/lua-5.4.7/lua.c" "${DEPS_DIR}/lua-5.4.7/luac.c" 
    "${DEPS_DIR}/lua-5.4.7/onelua.c" "${DEPS_DIR}/lua-5.4.7/ltests.c"
)
add_library(lua STATIC ${LUA_SOURCES})
target_include_directories(lua PUBLIC ${DEPS_DIR}/lua-5.4.7)

# SDL2 & Jolt
add_subdirectory(${DEPS_DIR}/SDL-release-2.30.11 ${CMAKE_BINARY_DIR}/sdl2 EXCLUDE_FROM_ALL)
set(USE_STATIC_MSVC_RUNTIME_LIBRARY OFF CACHE BOOL "" FORCE)
set(TARGET_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(CPP_RTTI_ENABLED ON CACHE BOOL "" FORCE)
add_subdirectory(${DEPS_DIR}/JoltPhysics-5.5.0/Build ${CMAKE_BINARY_DIR}/jolt EXCLUDE_FROM_ALL)

# --- StarlightEngine CORE LIBRARY ---
set(STARLIGHT_INCLUDE_DIR ${STARLIGHT_SDK_ROOT}/include)
set(STARLIGHT_SRC_DIR ${STARLIGHT_SDK_ROOT}/src)

add_library(glad STATIC ${STARLIGHT_INCLUDE_DIR}/glad/src/glad.c)
target_include_directories(glad PUBLIC ${STARLIGHT_INCLUDE_DIR}/glad/include)

file(GLOB_RECURSE CORE_SOURCES 
    "${STARLIGHT_SRC_DIR}/ai/*.cpp"
    "${STARLIGHT_SRC_DIR}/audio/*.cpp"
    "${STARLIGHT_SRC_DIR}/core/*.cpp"
    "${STARLIGHT_SRC_DIR}/ecs/*.cpp"
    "${STARLIGHT_SRC_DIR}/gui/*.cpp"
    "${STARLIGHT_SRC_DIR}/network/*.cpp"
    "${STARLIGHT_SRC_DIR}/physics/*.cpp"
    "${STARLIGHT_SRC_DIR}/renderer/*.cpp"
    "${STARLIGHT_SDK_ROOT}/thirdparty/wicked/core/wiJobSystem.cpp"
)

file(GLOB IMGUI_SOURCES "${STARLIGHT_SDK_ROOT}/thirdparty/imgui/*.cpp")

# Disable code analysis on third-party source files
set_source_files_properties(${IMGUI_SOURCES} PROPERTIES COMPILE_FLAGS "/analyze-")

add_library(StarlightCore STATIC ${CORE_SOURCES} ${IMGUI_SOURCES})
target_include_directories(StarlightCore PUBLIC 
    ${STARLIGHT_INCLUDE_DIR}
    ${STARLIGHT_SRC_DIR}
    ${STARLIGHT_SDK_ROOT}/thirdparty
    ${STARLIGHT_SDK_ROOT}/thirdparty/imgui
    ${STARLIGHT_SDK_ROOT}/thirdparty/pl_mpeg
    ${STARLIGHT_SDK_ROOT}/thirdparty/nlohmann
    ${STARLIGHT_SDK_ROOT}/thirdparty/wicked/core
)
target_link_libraries(StarlightCore PUBLIC 
    SDL2::SDL2main SDL2::SDL2-static
    glad Jolt glm entt sol2 lua meshoptimizer cgltf 
    ozz_animation ozz_base ozz_geometry
)
target_compile_definitions(StarlightCore PUBLIC SDL_MAIN_HANDLED UNICODE _UNICODE NOMINMAX _CRT_SECURE_NO_WARNINGS GLM_ENABLE_EXPERIMENTAL)
target_compile_options(StarlightCore PUBLIC /wd6385 /wd6386 /wd6011 /wd6262 /wd26812 /wd28182 /wd28159 /wd6255)

# --- SHOWCASE TARGET ---
add_executable(StarlightShowcase 
    ${STARLIGHT_SDK_ROOT}/examples/StarlightShowcase/main.cpp
)
target_link_libraries(StarlightShowcase PRIVATE StarlightCore)


if(MSVC)
    target_compile_options(StarlightCore PRIVATE /W4 /EHsc /FS /utf-8 /wd5321 /bigobj)
endif()
