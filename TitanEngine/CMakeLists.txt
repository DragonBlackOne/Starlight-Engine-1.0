cmake_minimum_required(VERSION 3.20)
project(TitanEngine VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(MSVC_RUNTIME "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
set(CMAKE_MSVC_RUNTIME_LIBRARY ${MSVC_RUNTIME})

set(TITAN_SDK_ROOT ${CMAKE_CURRENT_LIST_DIR})

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

# --- TitanEngine CORE LIBRARY ---
set(TITAN_INCLUDE_DIR ${TITAN_SDK_ROOT}/include)
set(TITAN_SRC_DIR ${TITAN_SDK_ROOT}/src)

add_library(glad STATIC ${TITAN_INCLUDE_DIR}/glad/src/glad.c)
target_include_directories(glad PUBLIC ${TITAN_INCLUDE_DIR}/glad/include)

file(GLOB_RECURSE CORE_SOURCES 
    "${TITAN_SRC_DIR}/ai/*.cpp"
    "${TITAN_SRC_DIR}/audio/*.cpp"
    "${TITAN_SRC_DIR}/core/*.cpp"
    "${TITAN_SRC_DIR}/ecs/*.cpp"
    "${TITAN_SRC_DIR}/gui/*.cpp"      # <--- Adicionado gui/ para DashboardSystem
    "${TITAN_SRC_DIR}/network/*.cpp"
    "${TITAN_SRC_DIR}/physics/*.cpp"
    "${TITAN_SRC_DIR}/renderer/*.cpp"
    "${TITAN_SDK_ROOT}/thirdparty/wicked/core/wiJobSystem.cpp"
)

file(GLOB IMGUI_SOURCES "${TITAN_SDK_ROOT}/thirdparty/imgui/*.cpp")

add_library(TitanCore STATIC ${CORE_SOURCES} ${IMGUI_SOURCES})
target_include_directories(TitanCore PUBLIC 
    ${TITAN_INCLUDE_DIR}
    ${TITAN_SRC_DIR}
    ${TITAN_SDK_ROOT}/thirdparty
    ${TITAN_SDK_ROOT}/thirdparty/imgui
    ${TITAN_SDK_ROOT}/thirdparty/pl_mpeg
    ${TITAN_SDK_ROOT}/thirdparty/nlohmann
    ${TITAN_SDK_ROOT}/thirdparty/wicked/core
    ${DEPS_DIR}/JoltPhysics-master
)
target_link_libraries(TitanCore PUBLIC 
    SDL2::SDL2main SDL2::SDL2-static
    glad Jolt glm entt sol2 lua meshoptimizer cgltf
)
target_compile_definitions(TitanCore PUBLIC SDL_MAIN_HANDLED UNICODE _UNICODE NOMINMAX _CRT_SECURE_NO_WARNINGS GLM_ENABLE_EXPERIMENTAL)

# --- SHOWCASE TARGET ---
if(EXISTS "${TITAN_SDK_ROOT}/examples/TitanShowcase")
    add_executable(TitanShowcase 
        ${TITAN_SDK_ROOT}/examples/TitanShowcase/main.cpp
        ${TITAN_SDK_ROOT}/examples/TitanShowcase/GameSuite.cpp
    )
    target_link_libraries(TitanShowcase PRIVATE TitanCore)
endif()

if(MSVC)
    target_compile_options(TitanCore PRIVATE /W4 /EHsc /FS /utf-8 /wd5321)
endif()
