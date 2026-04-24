cmake_minimum_required(VERSION 3.20)
project(StarlightEngine VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(MSVC_RUNTIME "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
set(CMAKE_MSVC_RUNTIME_LIBRARY ${MSVC_RUNTIME})

set(STARLIGHT_SDK_ROOT ${CMAKE_CURRENT_LIST_DIR})

# --- Open Source Dependencies ---
include(FetchContent)
FetchContent_Declare(
    meshoptimizer
    GIT_REPOSITORY https://github.com/zeux/meshoptimizer.git
    GIT_TAG        v0.21
    GIT_SHALLOW    TRUE
)
set(MESHOPT_BUILD_DEMO OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(meshoptimizer)

FetchContent_Declare(
    cgltf
    GIT_REPOSITORY https://github.com/jkuhlmann/cgltf.git
    GIT_TAG        v1.14
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(cgltf)
add_library(cgltf INTERFACE)
target_include_directories(cgltf INTERFACE ${cgltf_SOURCE_DIR})

set(DEPS_DIR "d:/deps")

# GLM, EnTT, Sol2, Lua
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE ${DEPS_DIR}/glm-1.0.1)
add_library(entt INTERFACE)
target_include_directories(entt INTERFACE ${DEPS_DIR}/entt-3.13.2/src)
add_library(sol2 INTERFACE)
target_include_directories(sol2 INTERFACE ${DEPS_DIR}/sol2-3.3.0/include)

file(GLOB LUA_SOURCES "${DEPS_DIR}/lua-5.4.6/*.c")
list(REMOVE_ITEM LUA_SOURCES 
    "${DEPS_DIR}/lua-5.4.6/lua.c" "${DEPS_DIR}/lua-5.4.6/luac.c" 
    "${DEPS_DIR}/lua-5.4.6/onelua.c" "${DEPS_DIR}/lua-5.4.6/ltests.c"
)
add_library(lua STATIC ${LUA_SOURCES})
target_include_directories(lua PUBLIC ${DEPS_DIR}/lua-5.4.6)

# SDL2 & Jolt
add_subdirectory(${DEPS_DIR}/SDL-release-2.30.0 ${CMAKE_BINARY_DIR}/sdl2 EXCLUDE_FROM_ALL)
set(USE_STATIC_MSVC_RUNTIME_LIBRARY OFF CACHE BOOL "" FORCE)
set(TARGET_UNIT_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(${DEPS_DIR}/JoltPhysics-master/Build ${CMAKE_BINARY_DIR}/jolt EXCLUDE_FROM_ALL)

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

add_library(StarlightCore STATIC ${CORE_SOURCES} ${IMGUI_SOURCES})
target_include_directories(StarlightCore PUBLIC 
    ${STARLIGHT_INCLUDE_DIR}
    ${STARLIGHT_SRC_DIR}
    ${STARLIGHT_SDK_ROOT}/thirdparty
    ${STARLIGHT_SDK_ROOT}/thirdparty/imgui
    ${STARLIGHT_SDK_ROOT}/thirdparty/pl_mpeg
    ${STARLIGHT_SDK_ROOT}/thirdparty/nlohmann
    ${STARLIGHT_SDK_ROOT}/thirdparty/wicked/core
    ${DEPS_DIR}/JoltPhysics-master
)
target_link_libraries(StarlightCore PUBLIC 
    SDL2::SDL2main SDL2::SDL2-static
    glad Jolt glm entt sol2 lua meshoptimizer cgltf
)
target_compile_definitions(StarlightCore PUBLIC SDL_MAIN_HANDLED UNICODE _UNICODE NOMINMAX _CRT_SECURE_NO_WARNINGS GLM_ENABLE_EXPERIMENTAL)

# --- SHOWCASE TARGET ---
if(EXISTS "${STARLIGHT_SDK_ROOT}/examples/StarlightShowcase")
    add_executable(StarlightShowcase 
        ${STARLIGHT_SDK_ROOT}/examples/StarlightShowcase/main.cpp
        ${STARLIGHT_SDK_ROOT}/examples/StarlightShowcase/GameSuite.cpp
    )
    target_link_libraries(StarlightShowcase PRIVATE StarlightCore)
endif()

if(MSVC)
    target_compile_options(StarlightCore PRIVATE /W4 /EHsc /FS /utf-8 /wd5321)
endif()
